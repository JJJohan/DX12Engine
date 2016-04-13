#include "DX12Renderer.h"
#include "HeapManager.h"
#include "CommandQueue.h"
#include "Material.h"
#include "RenderObject.h"
#include "GBuffer.h"
#include "Texture.h"

namespace Engine
{
	DX12Renderer* DX12Renderer::_instance = nullptr;

	DX12Renderer::DX12Renderer()
		: _featureInfo()
		, _pCamera(nullptr)
		, _pGBuffer(nullptr)
		, _pDepthBuffer(nullptr)
		, _scissorRect()
		, _swapChain(nullptr)
		, _device(nullptr)
		, _commandAllocator(nullptr)
		, _commandQueue(nullptr)
		, _rootSignature(nullptr)
		, _rtvHeap(nullptr)
		, _dsvHeap(nullptr)
		, _commandList(nullptr)
		, _isRendering(false)
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
		delete _pGBuffer;
		delete _pDepthBuffer;

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
		_dsvHeap.Reset();
		_fence.Reset();

#if _DEBUG
		//_debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
#endif
	}

	bool DX12Renderer::InitWindow(int width, int height, bool windowed, HWND windowHandle)
	{
		IRenderer::InitWindow(width, height, windowed, windowHandle);

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

	bool DX12Renderer::IsRendering() const
	{
		return _isRendering;
	}

	bool DX12Renderer::Render()
	{
		//_previousTime = std::chrono::high_resolution_clock::now();
		_renderFinished = false;

		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		LOGFAILEDCOMRETURN(_commandAllocator->Reset(), EXIT_FAILURE);

		// Check if the camera has moved.
		if (_pCamera->Update()) { }

		// Update objects
		for (auto it = RenderObject::_renderObjects.begin(); it != RenderObject::_renderObjects.end(); ++it)
		{
			(*it)->Update();
		}

		// Execute any queued GPU tasks.
		_isRendering = true;
		std::vector<ID3D12CommandList*> commandLists = CommandQueue::Process(_device.Get());
		if (!commandLists.empty())
		{
			_commandQueue->ExecuteCommandLists(UINT(commandLists.size()), &commandLists[0]);
		}

		// Begin the render process.
		PopulateCommandList();
		ID3D12CommandList* ppCommandLists[] = {_commandList.Get()};
		_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		_isRendering = false;

		// Present the frame.
		LOGFAILEDCOMRETURN(_swapChain->Present(_vsync, 0), EXIT_FAILURE);

		WaitForPreviousFrame();

		// Clear static upload buffers.
		HeapManager::UpdateHeaps();

		// Resize if neccessary.
		if (_resize)
		{
			ResizeRenderer();
			_resize = false;
		}

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
				_device->SetStablePowerState(true);
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
			ComPtr<IDXGIAdapter3> warpAdapter;
			LOGFAILEDCOMRETURN(
				factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)),
				EXIT_FAILURE);

			LOGFAILEDCOMRETURN(
				D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device)),
				EXIT_FAILURE);

			AdapterInfo adapterInfo = FeatureSupport::QueryAdapterInfo(warpAdapter.Get());
			_deviceName = adapterInfo.Name;
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
		}

		// Initialise the utility class.
		D3DUtils::Initialise(_device.Get());

#if _DEBUG
		// Get debug device for memory reporting.
		_device.As(&_debugDevice);
#endif

		// Populate feature level info.
		_featureInfo = FeatureSupport::QueryDeviceFeatures(_device.Get(), DXGI_FORMAT_R8G8B8A8_UNORM);
		_featureLevel = FeatureSupport::GetFeatureLevelString(_featureInfo.MaxFeatureLevelSupported);

		SystemInfo::PrintSystemInfo();
		Logging::Log("Creating Direct3D 12 device using adapter '" + _deviceName + "'.");

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

		// Create RTV for swap chain.
		CreateRTV();

		// Describe and create a CBV/SRV heap for constants and textures.
		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvHeapDesc = {};
		cbvSrvHeapDesc.NumDescriptors = ResourceFactory::CBufferLimit + ResourceFactory::TextureLimit;
		cbvSrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		LOGFAILEDCOMRETURN(
			_device->CreateDescriptorHeap(&cbvSrvHeapDesc, IID_PPV_ARGS(&_cbvSrvHeap)),
			EXIT_FAILURE);
		
		// Describe and create a depth stencil view (DSV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		LOGFAILEDCOM(_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap)));

		LOGFAILEDCOMRETURN(
			_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)),
			EXIT_FAILURE);

		HeapManager::SetDevice(_device.Get());
		_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(&_commandList));

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
			ID3DBlob* error = nullptr;
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			if (!D3DUtils::Succeeded(error))
			{
				return EXIT_FAILURE;
			}

			LOGFAILEDCOMRETURN(
				_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)),
				EXIT_FAILURE);
		}

		//_commandAllocator->Reset();
		//_commandList->Reset(_commandAllocator.Get(), nullptr);

		// Initialise factory pointers.
		ResourceFactory::_init(this, _cbvSrvHeap.Get());

		// Create depth buffer.
		_isRendering = true;
		CreateDepthBuffer();
		_isRendering = false;

		// Create GBuffer.
		_pGBuffer = new GBuffer(_device.Get(), _commandList.Get(), _cbvSrvHeap.Get(), _swapChain.Get());

		// Call the resource creation method.
		CommandQueue::Enqueue(_createMethod);
		//ID3D12CommandList* commandLists[] = { _commandList.Get() };
		_commandList->Close();
		//_commandQueue->ExecuteCommandLists(1, commandLists);
		std::vector<ID3D12CommandList*> commandLists = CommandQueue::Process(_device.Get());
		commandLists.push_back(_commandList.Get());
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

		CD3DX12_GPU_DESCRIPTOR_HANDLE rootHandle(ppHeaps[0]->GetGPUDescriptorHandleForHeapStart());
		_commandList->SetGraphicsRootDescriptorTable(0, rootHandle);
		rootHandle.Offset(ResourceFactory::CBufferLimit, D3DUtils::GetSRVDescriptorSize());
		_commandList->SetGraphicsRootDescriptorTable(1, rootHandle);

		_commandList->RSSetScissorRects(1, &_scissorRect);
		Material::ClearPSOHistory();

		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		_pGBuffer->Write();
		_pGBuffer->Clear();

		// Execute the draw loop function.
		_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//_drawLoop();

		_pGBuffer->Present();
		_pGBuffer->DrawTextures();

		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		_commandList->Close();
	}

	void DX12Renderer::Sync()
	{
		_syncMutex.lock();

		if (_fence.Get() == nullptr)
		{
			LOGFAILEDCOM(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));

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

		// Calculate FPS.
		UINT64 gpu, cpu, freq;
		_commandQueue->GetTimestampFrequency(&freq);
		_commandQueue->GetClockCalibration(&gpu, &cpu);
		UINT64 fps = freq / (gpu - _gpuTimestep);
		float fpsInvert = 1.0f / float(fps);
		Time::SetGPUDelta(fpsInvert);
		_gpuTimestep = gpu;

		// Signal and increment the fence value.
		const UINT64 fence = _fenceValue;
		LOGFAILEDCOMRETURN(_commandQueue->Signal(_fence.Get(), fence), EXIT_FAILURE);
		_fenceValue++;

		// Wait until the previous frame is finished.
		if (_fence->GetCompletedValue() < fence)
		{
			LOGFAILEDCOMRETURN(
				_fence->SetEventOnCompletion(fence, _fenceEvent),
				EXIT_FAILURE);
			WaitForSingleObject(_fenceEvent, INFINITE);
		}

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();
		_renderFinished = true;

		return EXIT_SUCCESS;
	}

	void DX12Renderer::CreateDepthBuffer()
	{
		// Create a resource description for the depth texture.
		D3D12_RESOURCE_DESC depthDesc = {};
		depthDesc.MipLevels = 1;
		depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		depthDesc.Alignment = 0;
		depthDesc.Width = _screenWidth;
		depthDesc.Height = _screenHeight;
		depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		depthDesc.DepthOrArraySize = 1;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_CLEAR_VALUE* clearVal = new D3D12_CLEAR_VALUE();
		clearVal->Format = DXGI_FORMAT_D32_FLOAT;
		clearVal->DepthStencil.Depth = 1.0f;
		clearVal->DepthStencil.Stencil = 0;

		// Create a depth stencil view description.
		D3D12_DEPTH_STENCIL_VIEW_DESC stencilDesc = {};
		stencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		stencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		stencilDesc.Texture2D.MipSlice = 0;

		Texture* t = ResourceFactory::CreateTexture(_screenWidth, _screenHeight);
		t->SetResourceDescription(depthDesc);
		t->SetHeapDescription(D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_DEPTH_WRITE, clearVal);
		t->Apply();

		_device->CreateDepthStencilView(t->GetResource(), &stencilDesc, _dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	void DX12Renderer::CreateRTV()
	{
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
			rtvHandle.Offset(1, D3DUtils::GetRTVDescriptorSize());
		}

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();
	}

	void DX12Renderer::BindBackBuffer() const
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		rtvHandle.Offset(_frameIndex, D3DUtils::GetRTVDescriptorSize());
		_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		
		// Clear the depth buffer.
		_commandList->ClearDepthStencilView(_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &_scissorRect);

		const float clearColor[] = { 0.0f, 0.0f, 0.2f, 1.0f };
		_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	void DX12Renderer::ResizeRenderer()
	{
		// Resize camera and scissor rect.
		_pCamera->Resize(float(_screenWidth), float(_screenHeight));
		_scissorRect.right = LONG(_screenWidth);
		_scissorRect.bottom = LONG(_screenHeight);

		// Reset render targets if they exist.
		for (int i = 0; i < _frameCount; ++i)
		{
			if (_renderTargets[i] != nullptr)
			{
				_renderTargets[i].Reset();
			}
		}

		// Reset heap if it exists.
		if (_rtvHeap != nullptr)
		{
			_rtvHeap.Reset();
		}

		// Resize swapchain.
		_swapChain->ResizeBuffers(_frameCount, UINT(_screenWidth), UINT(_screenHeight), DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		// Recreate the RTV.
		CreateRTV();
	}

	ID3D12DescriptorHeap* DX12Renderer::GetDepthBufferHeap() const
	{
		return _dsvHeap.Get();
	}
}

