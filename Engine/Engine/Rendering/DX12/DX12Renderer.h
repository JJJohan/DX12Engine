#pragma once

#include "../IRenderer.h"
#include "FeatureSupport.h"
#include <dxgi1_5.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "Camera.h"
#include <mutex>

using namespace Microsoft::WRL;
using namespace DirectX;

namespace Engine
{
	class RenderObject;

	class DX12Renderer : public IRenderer
	{
	public:
		DX12Renderer();
		virtual ~DX12Renderer();

		bool InitWindow(int width, int height, bool windowed) override;
		bool Update() override;
		bool Render() override;

		static DX12Renderer* Get();

		void Sync();

	private:
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter3** ppAdapter);

		static DX12Renderer* _instance;
		const static int _frameCount = 2;
		FeatureInfo _featureInfo;

		// Pipeline objects
		Camera* _pCamera;
		D3D12_RECT _scissorRect;
		ComPtr<IDXGISwapChain3> _swapChain;
		ComPtr<ID3D12Device> _device;
		ComPtr<ID3D12Resource> _renderTargets[_frameCount];
		ComPtr<ID3D12CommandAllocator> _commandAllocator;
		ComPtr<ID3D12CommandQueue> _commandQueue;
		ComPtr<ID3D12RootSignature> _rootSignature;
		ComPtr<ID3D12DescriptorHeap> _rtvHeap;
		ComPtr<ID3D12DescriptorHeap> _srvHeap;
		ComPtr<ID3D12GraphicsCommandList> _commandList;
#if _DEBUG
		ComPtr<ID3D12DebugDevice> _debugDevice;
#endif
		int _rtvDescriptorSize;
		bool _useWarpDevice;

		// Synchronization objects
		UINT _frameIndex;
		HANDLE _fenceEvent;
		ComPtr<ID3D12Fence> _fence;
		UINT64 _fenceValue;
		std::mutex _syncMutex;

		bool LoadPipeline();
		bool LoadAssets();
		void PopulateCommandList();
		bool WaitForPreviousFrame();

		friend class RenderObjectFactory;
		friend class MaterialFactory;
		friend class TextureFactory;
		friend class VertexBufferFactory;
		friend class IndexBufferFactory;
	};
}

