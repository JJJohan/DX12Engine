#include <D3Dcompiler.h>
#include "DX12Renderer.h"
#include "../../Utils/Helpers.h"
#include "d3dx12.h"
#include "CommandQueue.h"

namespace Engine
{
	DX12Renderer::DX12Renderer()
		: _pCamera(nullptr)
		, _swapChain(nullptr)
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
		, _pTriangle(nullptr)
		, _fenceValue(0)
	{
	}
	
	DX12Renderer::~DX12Renderer()
	{
		// Wait for the GPU to be done with all resources.
		WaitForPreviousFrame();

		CloseHandle(_fenceEvent);

		delete _pCamera;
		delete _pTriangle;
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

		// Create a camera
		_pCamera = Camera::CreateCamera(_device.Get(), XMFLOAT4(0.0f, 0.0f, float(width), float(height)), 90.0f, 0.0f, 1.0f);

		return EXIT_SUCCESS;
	}

	bool DX12Renderer::Update()
	{
		return EXIT_SUCCESS;
	}

	bool DX12Renderer::Render()
	{
		_renderFinished = false;

		// Record all the commands we need to render the scene into the command list.
		PopulateCommandList();

		// Execute the command list.
		ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };
		_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Check if the camera has moved.
		if (_pCamera->Update())
		{
			
		}
		
		// Present the frame.
		LOGFAILEDCOMRETURN(
			_swapChain->Present(_vsync, 0),
			EXIT_FAILURE);

		WaitForPreviousFrame();

		return EXIT_SUCCESS;
	}

	void DX12Renderer::GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter3** ppAdapter)
	{
		*ppAdapter = nullptr;
		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			IDXGIAdapter* pAdapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters(adapterIndex, &pAdapter))
			{
				// No more adapters to enumerate.
				break;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				*ppAdapter = static_cast<IDXGIAdapter3*>(pAdapter);
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
		LOGFAILEDRETURN(
			CreateDXGIFactory1(IID_PPV_ARGS(&factory)),
			std::string(Logging::GetWin32ErrorString()),
			EXIT_FAILURE);

		if (_useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			LOGFAILEDCOMRETURN(
				factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)),
				EXIT_FAILURE);

			LOGFAILEDCOMRETURN(
				D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device)),
				EXIT_FAILURE);
		}
		else
		{
			ComPtr<IDXGIAdapter3> hardwareAdapter;
			GetHardwareAdapter(factory.Get(), &hardwareAdapter);
			if (hardwareAdapter.Get() == nullptr)
			{
				Logging::LogError("Could not find a device interface that meets the feature level requirements of DirectX 12 on this system.");
				return EXIT_FAILURE;
			}

			AdapterInfo adapterInfo = FeatureSupport::QueryAdapterInfo(hardwareAdapter.Get());
			_deviceName = adapterInfo.Name;
			
			DXGI_QUERY_VIDEO_MEMORY_INFO videoMemInfo = {};
			LOGFAILEDCOMRETURN(
				hardwareAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemInfo),
				EXIT_FAILURE);
			_deviceMemoryTotal = adapterInfo.DedicatedMemory;
			_deviceMemoryFree = (videoMemInfo.Budget - videoMemInfo.CurrentUsage) / 1024 / 1024;

			Logging::Log("Creating Direct3D12 device using adapter '" + adapterInfo.Name + "'.");
			LOGFAILEDCOMRETURN(
				D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device)),
				EXIT_FAILURE);
		}

		// Populate feature level info.
		_featureInfo = FeatureSupport::QueryDeviceFeatures(_device.Get(), DXGI_FORMAT_R8G8B8A8_UNORM);
		_featureLevel = FeatureSupport::GetFeatureLevelString(_featureInfo.MaxFeatureLevelSupported);

		// Describe and create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		LOGFAILEDCOMRETURN(
			_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue)),
			EXIT_FAILURE);

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
		LOGFAILEDCOMRETURN(
			factory->CreateSwapChain(_commandQueue.Get(), &swapChainDesc, &swapChain),
			EXIT_FAILURE);

		LOGFAILEDCOMRETURN(
			swapChain.As(&_swapChain),
			EXIT_FAILURE);

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();

		// Create descriptor heaps.
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = _frameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		LOGFAILEDCOMRETURN(
			_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap)),
			EXIT_FAILURE);

		_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Create frame resources.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < _frameCount; n++)
		{
			LOGFAILEDCOMRETURN(
				_swapChain->GetBuffer(n, IID_PPV_ARGS(&_renderTargets[n])),
				EXIT_FAILURE);

			_device->CreateRenderTargetView(_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, _rtvDescriptorSize);
		}	

		LOGFAILEDCOMRETURN(
			_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)),
			EXIT_FAILURE);

		return EXIT_SUCCESS;
	}

	bool DX12Renderer::LoadAssets()
	{
		// Create an empty root signature.
		{
			// define descriptor tables for a CBV for shaders
			CD3DX12_DESCRIPTOR_RANGE DescRange[1];
			DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

			CD3DX12_ROOT_PARAMETER rootParameters[1];
			rootParameters[0].InitAsConstantBufferView(0);

			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(1, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			LOGFAILEDCOMRETURN(
				D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error),
				EXIT_FAILURE);

			LOGFAILEDCOMRETURN(
				_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)),
				EXIT_FAILURE);
		}

		// Create an example triangle object.
		std::vector<Vertex> triangleVertices =
		{
			{ { 0.0f, 0.25f * _aspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },
			{ { 0.25f, -0.25f * _aspectRatio, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },
			{ { -0.25f, -0.25f * _aspectRatio, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } }
		};

		_pTriangle = new RenderObject();
		_pTriangle->SetVertices(_device.Get(), triangleVertices);
		_pTriangle->LoadVertexShader(GetRelativeFilePath("Shaders\\Diffuse.hlsl"), "VSMain", "vs_5_1");
		_pTriangle->LoadPixelShader(GetRelativeFilePath("Shaders\\Diffuse.hlsl"), "PSMain", "ps_5_1");

		// Create the pipeline state, which includes compiling and loading shaders.
		{
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
			psoDesc.VS = { reinterpret_cast<UINT8*>(_pTriangle->GetVertexShader()->GetBufferPointer()), _pTriangle->GetVertexShader()->GetBufferSize() };
			psoDesc.PS = { reinterpret_cast<UINT8*>(_pTriangle->GetPixelShader()->GetBufferPointer()), _pTriangle->GetPixelShader()->GetBufferSize() };
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;

			LOGFAILEDCOMRETURN(
				_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState)),
				EXIT_FAILURE);
		}

		// Create the command list.
		LOGFAILEDCOMRETURN(
			_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), _pipelineState.Get(), IID_PPV_ARGS(&_commandList)),
			EXIT_FAILURE);

		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		LOGFAILEDCOMRETURN(
			_commandList->Close(),
			EXIT_FAILURE);

		// Create synchronization objects and wait until assets have been uploaded to the GPU.
		{
			LOGFAILEDCOMRETURN(
				_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)),
				EXIT_FAILURE);
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
		LOGFAILEDCOMRETURN(
			_commandAllocator->Reset(),
			EXIT_FAILURE);

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before 
		// re-recording.
		LOGFAILEDCOMRETURN(
			_commandList->Reset(_commandAllocator.Get(), _pipelineState.Get()),
			EXIT_FAILURE);

		// Set necessary state.
		_commandList->SetGraphicsRootSignature(_rootSignature.Get());
		_commandList->RSSetViewports(1, &_pCamera->GetViewPort());
		_commandList->RSSetScissorRects(1, &_scissorRect);

		// Point to the camera's constant buffer for camera transformation data.
		_commandList->SetGraphicsRootConstantBufferView(0, _pCamera->GetCBuffer()->GetGPUVirtualAddress());

		// Indicate that the back buffer will be used as a render target.
		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), 
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart(), _frameIndex, _rtvDescriptorSize);
		_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Record commands.
		_commandList->ClearRenderTargetView(rtvHandle, _clearColour, 0, nullptr);
		_pTriangle->Draw(_commandList.Get());

		// Indicate that the back buffer will now be used to present.
		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), 
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		LOGFAILEDCOMRETURN(
			_commandList->Close(),
			EXIT_FAILURE);

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
		LOGFAILEDCOMRETURN(
			_commandQueue->Signal(_fence.Get(), fence),
			EXIT_FAILURE);
		_fenceValue++;

		// Wait until the previous frame is finished.
		if (_fence->GetCompletedValue() < fence)
		{
			LOGFAILEDCOMRETURN(
				_fence->SetEventOnCompletion(fence, _fenceEvent),
				EXIT_FAILURE);
			WaitForSingleObject(_fenceEvent, INFINITE);
		}

		CommandQueue::Process();

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();
		_renderFinished = true;

		return EXIT_SUCCESS;
	}
}
