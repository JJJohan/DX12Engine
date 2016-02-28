#pragma once

namespace Engine
{
	class Texture;

	class GBuffer
	{
	public:
		GBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvHeap);
		~GBuffer();

		ID3D12DescriptorHeap* GetRTVHeap() const;
		ID3D12DescriptorHeap* GetDSVHeap() const;

		void Bind() const;
		void Present() const;
		void Clear();
		void Resize();

		enum GBUFFER_TEXTURE_TYPE 
		{
			GBUFFER_TEXTURE_TYPE_POSITION,
			GBUFFER_TEXTURE_TYPE_DIFFUSE,
			//GBUFFER_TEXTURE_TYPE_NORMAL,
			GBUFFER_TEXTURE_TYPE_TEXCOORD,
			GBUFFER_NUM_TEXTURES
		};

	private:
		

		void CreateHeaps();
		void CreateTextures();

		bool _initialised;

		ID3D12DescriptorHeap* _pRtvHeap;
		ID3D12DescriptorHeap* _pDsvHeap;
		ID3D12DescriptorHeap* _pSrvHeap;

		ID3D12Device* _pDevice;
		ID3D12GraphicsCommandList* _pCommandList;

		CD3DX12_CPU_DESCRIPTOR_HANDLE _rtvHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE _srvHandle;

		Texture* _pTextures[GBUFFER_NUM_TEXTURES];
		Texture* _pDepthTexture;
		
		int _screenWidth;
		int _screenHeight;
		D3D12_RECT _screenRect;
	};
}

