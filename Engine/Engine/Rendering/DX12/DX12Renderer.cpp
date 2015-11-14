#include <D3Dcompiler.h>
#include "DX12Renderer.h"
#include "../../Utils/Helpers.h"
#include <sstream>

namespace Engine
{
	DX12Renderer::DX12Renderer()
		: _swapChain(nullptr)
		, _device(nullptr)
		, _commandAllocator(nullptr)
		, _commandQueue(nullptr)
		, _rootSignature(nullptr)
		, _rtvHeap(nullptr)
		, _pipelineState(nullptr)
		, _commandList(nullptr)
		, _rtvDescriptorSize(0)
		, _useWarpDevice(false)
		, _vertexBuffer(nullptr)
		, _frameIndex(0)
		, _fenceEvent(nullptr)
		, _fence(nullptr)
		, _fenceValue(0)
	{
	}
	
	DX12Renderer::~DX12Renderer()
	{
		// Wait for the GPU to be done with all resources.
		WaitForPreviousFrame();

		CloseHandle(_fenceEvent);

		_swapChain.Reset();
		_device.Reset();
		_commandAllocator.Reset();
		_commandQueue.Reset();
		_rootSignature.Reset();
		_rtvHeap.Reset();
		_pipelineState.Reset();
		_commandList.Reset();
		_vertexBuffer.Reset();
		_fence.Reset();
	}

	bool DX12Renderer::InitWindow(int width, int height, bool windowed)
	{
		IRenderer::InitWindow(width, height, windowed);

		// Set up the viewports
		_viewport.Width = float(_screenWidth);
		_viewport.Height = float(_screenHeight);
		_viewport.MaxDepth = 1.0f;

		_scissorRect.right = _screenWidth;
		_scissorRect.bottom = _screenHeight;

		if (LoadPipeline() == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		if (LoadAssets() == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	bool DX12Renderer::Update()
	{
		return EXIT_SUCCESS;
	}

	bool DX12Renderer::Render()
	{
		if (IRenderer::Render() == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		// Record all the commands we need to render the scene into the command list.
		PopulateCommandList();

		// Execute the command list.
		ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };
		_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Present the frame.
		if (Failed(_swapChain->Present(_vsync, 0)))
		{
			return EXIT_FAILURE;
		}

		WaitForPreviousFrame();

		return EXIT_SUCCESS;
	}

	void DX12Renderer::GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
	{
		*ppAdapter = nullptr;
		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			IDXGIAdapter1* pAdapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter))
			{
				// No more adapters to enumerate.
				break;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				*ppAdapter = pAdapter;
				return;
			}
			pAdapter->Release();
		}
	}

	bool DX12Renderer::LoadPipeline()
	{
#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
#endif

		ComPtr<IDXGIFactory4> factory;
		if (Failed(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
		{
			return EXIT_FAILURE;
		}

		if (_useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			if (Failed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
			{
				return EXIT_FAILURE;
			}

			if (Failed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device))))
			{
				return EXIT_FAILURE;
			}
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(factory.Get(), &hardwareAdapter);
			if (hardwareAdapter.Get() == nullptr)
			{
				Logging::LogError("Could not find a device interface that meets the feature level requirements of DirectX 12 on this system.");
				return EXIT_FAILURE;
			}

			DXGI_ADAPTER_DESC1 desc;
			hardwareAdapter->GetDesc1(&desc);
			std::wstring wideDesc = std::wstring(desc.Description);
			std::stringstream mem;
			mem << "Available graphics memory: " << desc.DedicatedVideoMemory << " MB";
			Logging::Log("Creating Direct3D12 device using adapter '" + std::string(wideDesc.begin(), wideDesc.end()) + "'.");
			Logging::Log(mem.str());

			if (Failed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device))))
			{
				return EXIT_FAILURE;
			}
		}

		// Describe and create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (Failed(_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue))))
		{
			return EXIT_FAILURE;
		}

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = _frameCount;
		swapChainDesc.BufferDesc.Width = _screenWidth;
		swapChainDesc.BufferDesc.Height = _screenHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.OutputWindow = _windowHandle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = _windowed;

		// Swap chain needs the queue so that it can force a flush on it.
		ComPtr<IDXGISwapChain> swapChain;
		if (Failed(factory->CreateSwapChain(_commandQueue.Get(), &swapChainDesc, &swapChain)))
		{
			return EXIT_FAILURE;
		}

		if (Failed(swapChain.As(&_swapChain)))
		{
			return EXIT_FAILURE;
		}

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();

		// Create descriptor heaps.
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = _frameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (Failed(_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap))))
		{
			return EXIT_FAILURE;
		}

		_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Create frame resources.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < _frameCount; n++)
		{
			if (Failed(_swapChain->GetBuffer(n, IID_PPV_ARGS(&_renderTargets[n]))))
			{
				return EXIT_FAILURE;
			}

			_device->CreateRenderTargetView(_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, _rtvDescriptorSize);
		}	

		if (Failed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator))))
		{
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	bool DX12Renderer::LoadAssets()
	{
		// Create an empty root signature.
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			if (Failed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
			{
				return EXIT_FAILURE;
			}

			if (Failed(_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature))))
			{
				return EXIT_FAILURE;
			}
		}

		// Create the pipeline state, which includes compiling and loading shaders.
		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;

#ifdef _DEBUG
			// Enable better shader debugging with the graphics debugging tools.
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT compileFlags = 0;
#endif

			if (Failed(D3DCompileFromFile(L"C:\\Users\\Johan\\Documents\\Repositories\\Engine\\Engine\\Engine\\Shaders\\Diffuse.hlsl", 
				nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr)))
			{
				return EXIT_FAILURE;
			}

			if (Failed(D3DCompileFromFile(L"C:\\Users\\Johan\\Documents\\Repositories\\Engine\\Engine\\Engine\\Shaders\\Diffuse.hlsl", 
				nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr)))
			{
				return EXIT_FAILURE;
			}

			// Define the vertex input layout.
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			// Describe and create the graphics pipeline state object (PSO).
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = _rootSignature.Get();
			psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
			psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			if (Failed(_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState))))
			{
				return EXIT_FAILURE;
			}
		}

		// Create the command list.
		if (Failed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), _pipelineState.Get(), IID_PPV_ARGS(&_commandList))))
		{
			return EXIT_FAILURE;
		}

		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		if (Failed(_commandList->Close()))
		{
			return EXIT_FAILURE;
		}

		// Create the vertex buffer.
		{
			// Define the geometry for a triangle.
			Vertex triangleVertices[] =
			{
				{ { 0.0f, 0.25f * _aspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
				{ { 0.25f, -0.25f * _aspectRatio, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
				{ { -0.25f, -0.25f * _aspectRatio, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
			};

			const UINT vertexBufferSize = sizeof(triangleVertices);

			// Note: using upload heaps to transfer static data like vert buffers is not 
			// recommended. Every time the GPU needs it, the upload heap will be marshalled 
			// over. Please read up on Default Heap usage. An upload heap is used here for 
			// code simplicity and because there are very few verts to actually transfer.
			if (Failed(_device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&_vertexBuffer))))
			{
				return EXIT_FAILURE;
			}

			// Copy the triangle data to the vertex buffer.
			UINT8* pVertexDataBegin;
			if (Failed(_vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pVertexDataBegin))))
			{
				return EXIT_FAILURE;
			}
			memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
			_vertexBuffer->Unmap(0, nullptr);
			
			// Initialize the vertex buffer view.
			_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
			_vertexBufferView.StrideInBytes = sizeof(Vertex);
			_vertexBufferView.SizeInBytes = vertexBufferSize;
		}

		// Create synchronization objects and wait until assets have been uploaded to the GPU.
		{
			if (Failed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence))))
			{
				return EXIT_FAILURE;
			}
			_fenceValue = 1;

			// Create an event handle to use for frame synchronization.
			_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			if (_fenceEvent == nullptr)
			{
				return EXIT_FAILURE;
			}

			// Wait for the command list to execute; we are reusing the same command 
			// list in our main loop but for now, we just want to wait for setup to 
			// complete before continuing.
			if (WaitForPreviousFrame() == EXIT_FAILURE)
			{
				return EXIT_FAILURE;
			}
		}

		return EXIT_SUCCESS;
	}
	
	bool DX12Renderer::PopulateCommandList() const
	{
		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		if (Failed(_commandAllocator->Reset()))
		{
			return EXIT_FAILURE;
		}

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before 
		// re-recording.
		if (Failed(_commandList->Reset(_commandAllocator.Get(), _pipelineState.Get())))
		{
			return EXIT_FAILURE;
		}

		// Set necessary state.
		_commandList->SetGraphicsRootSignature(_rootSignature.Get());
		_commandList->RSSetViewports(1, &_viewport);
		_commandList->RSSetScissorRects(1, &_scissorRect);

		// Indicate that the back buffer will be used as a render target.
		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart(), _frameIndex, _rtvDescriptorSize);
		_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Record commands.
		_commandList->ClearRenderTargetView(rtvHandle, _clearColour, 0, nullptr);
		_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
		_commandList->DrawInstanced(3, 1, 0, 0);

		// Indicate that the back buffer will now be used to present.
		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		if (Failed(_commandList->Close()))
		{
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	bool DX12Renderer::WaitForPreviousFrame()
	{
		// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
		// This is code implemented as such for simplicity. More advanced samples 
		// illustrate how to use fences for efficient resource usage.

		if (_commandQueue == nullptr || _fence == nullptr)
		{
			return EXIT_FAILURE;
		}

		// Signal and increment the fence value.
		const UINT64 fence = _fenceValue;
		if (Failed(_commandQueue->Signal(_fence.Get(), fence)))
		{
			return EXIT_FAILURE;
		}
		_fenceValue++;

		// Wait until the previous frame is finished.
		if (_fence->GetCompletedValue() < fence)
		{
			if (Failed(_fence->SetEventOnCompletion(fence, _fenceEvent)))
			{
				return EXIT_FAILURE;
			}
			WaitForSingleObject(_fenceEvent, INFINITE);
		}

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();

		return EXIT_SUCCESS;
	}
}
