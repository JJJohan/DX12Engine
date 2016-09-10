#pragma once
#include "RenderObject.h"

struct IDXGISwapChain3;

namespace Engine
{
	class Texture;

	class GBuffer
	{
	public:
		GBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvHeap, IDXGISwapChain3* swapChain);
		~GBuffer();

		ID3D12DescriptorHeap* GetRTVHeap() const;

		void Present() const;
		void Clear(ID3D12DescriptorHeap* dsvHeap);
		void Resize();

		void DrawTextures();

		enum BufferType 
		{
			GBUFFER_BUFFER_TYPE_ALBEDO_ROUGHNESS,
			GBUFFER_BUFFER_TYPE_NORMAL_METALLIC,
			GBUFFER_BUFFER_TYPE_TEXCOORD,
			GBUFFER_BUFFER_TYPE_DEPTH,
			GBUFFER_NUM_BUFFERS
		};

		struct BufferFormat
		{
			BufferType Type;
			DXGI_FORMAT Format;
		};

		const static BufferFormat Buffers[GBUFFER_NUM_BUFFERS];

	private:
		void FinishResourceInit();
		void CreateHeaps();
		void CreateScreenQuad();
		void CreateTextures();

		bool _initialised;

		ID3D12DescriptorHeap* _pRtvHeap;
		ID3D12DescriptorHeap* _pSrvHeap;

		ID3D12Device* _pDevice;
		ID3D12GraphicsCommandList* _pCommandList;
		IDXGISwapChain3* _pSwapChain;

		CD3DX12_CPU_DESCRIPTOR_HANDLE _rtvHandle;

		Texture* _pTextures[GBUFFER_NUM_BUFFERS];
		RenderObject* _pScreenQuads[GBUFFER_NUM_BUFFERS];
		Material* _pScreenMaterial;
		
		int _screenWidth;
		int _screenHeight;
		D3D12_RECT _screenRect;
	};
}

