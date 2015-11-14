#pragma once

#include "../IRenderer.h"
#include "d3dx12.h"
#include <dxgi1_5.h>
#include <wrl/client.h>
#include <DirectXMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;

namespace Engine
{
	class DX12Renderer : public IRenderer
	{
	public:
		DX12Renderer();
		virtual ~DX12Renderer();

		bool InitWindow(int width, int height, bool windowed) override;
		bool Update() override;
		bool Render() override;

	private:
		void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter);

		const static int _frameCount = 2;

		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};

		// Pipeline objects
		D3D12_VIEWPORT _viewport;
		D3D12_RECT _scissorRect;
		ComPtr<IDXGISwapChain3> _swapChain;
		ComPtr<ID3D12Device> _device;
		ComPtr<ID3D12Resource> _renderTargets[_frameCount];
		ComPtr<ID3D12CommandAllocator> _commandAllocator;
		ComPtr<ID3D12CommandQueue> _commandQueue;
		ComPtr<ID3D12RootSignature> _rootSignature;
		ComPtr<ID3D12DescriptorHeap> _rtvHeap;
		ComPtr<ID3D12PipelineState> _pipelineState;
		ComPtr<ID3D12GraphicsCommandList> _commandList;
		int _rtvDescriptorSize;
		bool _useWarpDevice;

		// App resources
		ComPtr<ID3D12Resource> _vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

		// Synchronization objects
		UINT _frameIndex;
		HANDLE _fenceEvent;
		ComPtr<ID3D12Fence> _fence;
		UINT64 _fenceValue;
		
		bool LoadPipeline();
		bool LoadAssets();
		bool PopulateCommandList() const;
		bool WaitForPreviousFrame();
	};
}
