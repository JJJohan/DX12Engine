#include <D3Dcompiler.h>
#include "DX12Renderer.h"
#include "d3dx12.h"
#include "CommandQueue.h"
#include "Material.h"
#include "HeapManager.h"
#include "../../Utils/Helpers.h"
#include "../../Factory/ResourceFactory.h"
#include "RenderObject.h"
#include "../../Utils/SystemInfo.h"

namespace Engine
{
	DX12Renderer* DX12Renderer::_instance = nullptr;

	DX12Renderer::DX12Renderer()
		: _featureInfo()
		  , _pCamera(nullptr)
		  , _scissorRect()
		  , _swapChain(nullptr)
		  , _device(nullptr)
		  , _commandAllocator(nullptr)
		  , _commandQueue(nullptr)
		  , _rootSignature(nullptr)
		  , _rtvHeap(nullptr)
		  , _commandList(nullptr)
		  , _rtvDescriptorSize(0)
		  , _cbvSrvDescriptorSize(0)
		  , _useWarpDevice(false)
		  , _resize(false)
		  , _frameIndex(0)
		  , _fenceEvent(nullptr)
		  , _fence(nullptr)
		  , _fenceValue(0)
	{
		_instance = this;
	}

	DX12Renderer::~DX12Renderer()
	{
		// Wait for the GPU to be done with all resources.
		WaitForPreviousFrame();

		CloseHandle(_fenceEvent);

		delete _pCamera;

		HeapManager::ReleaseHeaps();
		Material::ReleasePSOCache();
		CommandQueue::Release();

		_swapChain.Reset();
		_device.Reset();
		_commandList.Reset();
		_commandAllocator.Reset();
		_commandQueue.Reset();
		_rootSignature.Reset();
		_rtvHeap.Reset();
		_fence.Reset();

#if _DEBUG
		//_debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
#endif
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
		_pCamera = Camera::CreateCamera(_device.Get(), float(width), float(height), 90.0f, 0.01f, 100.0f);

		return EXIT_SUCCESS;
	}

	bool DX12Renderer::Update()
	{
		return EXIT_SUCCESS;
	}

	bool DX12Renderer::Render()
	{
		_previousTime = std::chrono::high_resolution_clock::now();
		_renderFinished = false;

		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		LOGFAILEDCOMRETURN(_commandAllocator->Reset(), EXIT_FAILURE);

		// Check if the camera has moved.
		if (_pCamera->Update())
		{
		}

		// Update objects
		for (auto it = RenderObject::_renderObjects.begin(); it != RenderObject::_renderObjects.end(); ++it)
		{
			(*it)->Update();
		}

		// Execute any queued GPU tasks.
		std::vector<ID3D12CommandList*> commandLists = CommandQueue::Process(_device.Get());
		if (!commandLists.empty())
		{
			_commandQueue->ExecuteCommandLists(UINT(commandLists.size()), &commandLists[0]);
		}

		// Begin the render process.
		PopulateCommandList();
		ID3D12CommandList* ppCommandLists[] = {_commandList.Get()};
		_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Present the frame.
		LOGFAILEDCOMRETURN(_swapChain->Present(_vsync, 0), EXIT_FAILURE);

		// Resize if neccessary.
		if (_resize)
		{
			ResizeRenderer();
			_resize = false;
		}

		WaitForPreviousFrame();

		// Clear static upload buffers.
		HeapManager::UpdateHeaps();

		return EXIT_SUCCESS;
	}

	DX12Renderer* DX12Renderer::Get()
	{
		return _instance;
	}

	void DX12Renderer::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter3** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> adapter;
		*ppAdapter = nullptr;

		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device))))
			{
				break;
			}
		}

		*ppAdapter = static_cast<IDXGIAdapter3*>(adapter.Detach());
	}

	void DX12Renderer::Resize(float width, float height)
	{
		IRenderer::Resize(width, height);
		_resize = true;
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

			SystemInfo::PrintSystemInfo();
			Logging::Log("Creating Direct3D12 device using adapter '" + adapterInfo.Name + "'.");
		}

#if _DEBUG
		// Get debug device for memory reporting.
		_device.Get()->QueryInterface(IID_PPV_ARGS(&_debugDevice));
#endif

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
		LOGFAILEDCOMRETURN(factory->CreateSwapChain(_commandQueue.Get(), &swapChainDesc, &swapChain), EXIT_FAILURE);
		LOGFAILEDCOMRETURN(swapChain.As(&_swapChain), EXIT_FAILURE);
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

		// Describe and create a CBV/SRV heap for constants and textures.
		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvHeapDesc = {};
		cbvSrvHeapDesc.NumDescriptors = ResourceFactory::CBufferLimit + ResourceFactory::TextureLimit;
		cbvSrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		LOGFAILEDCOMRETURN(
			_device->CreateDescriptorHeap(&cbvSrvHeapDesc, IID_PPV_ARGS(&_cbvSrvHeap)),
			EXIT_FAILURE);

		_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_cbvSrvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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

		HeapManager::SetDevice(_device.Get());
		_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(&_commandList));
		_commandList->Close();

		return EXIT_SUCCESS;
	}

	bool DX12Renderer::LoadAssets()
	{
		// Create an empty root signature.
		{
			// define descriptor tables for a CBV for shaders
			CD3DX12_DESCRIPTOR_RANGE DescRange[2];
			DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, ResourceFactory::CBufferLimit, 0);
			DescRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, ResourceFactory::TextureLimit, 0);

			CD3DX12_ROOT_PARAMETER rootParameters[2];
			rootParameters[0].InitAsDescriptorTable(1, &DescRange[0], D3D12_SHADER_VISIBILITY_VERTEX);
			rootParameters[1].InitAsDescriptorTable(1, &DescRange[1], D3D12_SHADER_VISIBILITY_PIXEL);

			D3D12_STATIC_SAMPLER_DESC sampler = {};
			sampler.Filter = D3D12_FILTER_ANISOTROPIC;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.MipLODBias = 0;
			sampler.MaxAnisotropy = 16;
			sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler.MinLOD = 0.0f;
			sampler.MaxLOD = D3D12_FLOAT32_MAX;
			sampler.ShaderRegister = 0;
			sampler.RegisterSpace = 0;
			sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			LOGFAILEDCOMRETURN(
				D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error),
				EXIT_FAILURE);

			LOGFAILEDCOMRETURN(
				_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)),
				EXIT_FAILURE);
		}

		// Initialise factory pointers.
		ResourceFactory::_init(static_cast<DX12Renderer*>(this), _cbvSrvHeap.Get());

		// Call the resource creation method.
		CommandQueue::Enqueue(_createMethod);
		std::vector<ID3D12CommandList*> commandLists = CommandQueue::Process(_device.Get());
		_commandQueue->ExecuteCommandLists(UINT(commandLists.size()), &commandLists[0]);

		// Create synchronization objects and wait until assets have been uploaded to the GPU.
		Sync();

		return EXIT_SUCCESS;
	}

	void DX12Renderer::PopulateCommandList() const
	{
		ResourceFactory::AssignCommandList(_commandList.Get());
		_commandList->Reset(_commandAllocator.Get(), nullptr);

		// Set necessary state.
		_commandList->SetGraphicsRootSignature(_rootSignature.Get());

		// Point to CBVs and SRVs.
		ID3D12DescriptorHeap* ppHeaps[] = {_cbvSrvHeap.Get()};
		_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(_cbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), 0, _cbvSrvDescriptorSize);
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(_cbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), ResourceFactory::CBufferLimit, _cbvSrvDescriptorSize);
		_commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
		_commandList->SetGraphicsRootDescriptorTable(1, srvHandle);

		_commandList->RSSetViewports(1, &_pCamera->GetViewPort());
		_commandList->RSSetScissorRects(1, &_scissorRect);
		Material::ClearPSOHistory();

		// Indicate that the back buffer will be used as a render target.
		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(),
		                                                                       D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart(), _frameIndex, _rtvDescriptorSize);
		_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Record commands.
		_commandList->ClearRenderTargetView(rtvHandle, _clearColour, 0, nullptr);

		// Execute the draw loop function.
		_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_drawLoop();

		// Indicate that the back buffer will now be used to present.
		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(),
		                                                                       D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		_commandList->Close();
	}

	void DX12Renderer::Sync()
	{
		_syncMutex.lock();

		if (_fence.Get() == nullptr)
		{
			LOGFAILEDCOM(
				_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));

			_fenceValue = 1;
		}

		// Create an event handle to use for frame synchronization.
		_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		if (_fenceEvent == nullptr)
		{
			Logging::LogError("Failed to create GPU fence event handle.");
			return;
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		if (WaitForPreviousFrame() == EXIT_FAILURE)
		{
			Logging::LogError("Frame sync failed.");
		}

		_syncMutex.unlock();
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

		// Wait for FPS target if we're rendering too fast.
		if (_fpsLimit > 0.0f)
		{
			std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> diff = currentTime - _previousTime;
			float time = diff.count();
			if (time < _fpsLimit)
			{
				size_t timeLeft = size_t((_fpsLimit - time) * 1e3f);
				std::this_thread::sleep_for(std::chrono::milliseconds(timeLeft));
			}
		}

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();
		_renderFinished = true;

		return EXIT_SUCCESS;
	}

	void DX12Renderer::ResizeRenderer()
	{
		_pCamera->Resize(float(_screenWidth), float(_screenHeight));
		_scissorRect.right = LONG(_screenWidth);
		_scissorRect.bottom = LONG(_screenHeight);

		for (int i = 0; i < _frameCount; ++i)
		{
			_renderTargets[i].Reset();
		}
		_rtvHeap.Reset();
		_swapChain->ResizeBuffers(_frameCount, UINT(_screenWidth), UINT(_screenHeight), DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		// Create descriptor heaps.
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = _frameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		LOGFAILEDCOM(_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap)));

		// Create frame resources.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < _frameCount; n++)
		{
			LOGFAILEDCOM(_swapChain->GetBuffer(n, IID_PPV_ARGS(&_renderTargets[n])));

			_device->CreateRenderTargetView(_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, _rtvDescriptorSize);
		}
	}
}

