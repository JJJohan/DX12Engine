#pragma once
#include "../Rendering/DX12/VertexBuffer.h"

namespace Engine
{
	class DX12Renderer;
	class IndexBuffer;
	class Texture;
	class Material;
	class ConstantBuffer;

	class ResourceFactory
	{
	public:
		static void AssignCommandList(ID3D12CommandList* commandList);
		static ID3D12CommandList* GetCommandList();
		static ConstantBuffer* CreateConstantBuffer();
		static IndexBuffer* CreateIndexBuffer();
		static Material* CreateMaterial();
		static Texture* CreateTexture();
		static ID3D12DescriptorHeap* GetCbvSrvHeap();

		static int GetTextureSlot();
		static int GetCBufferSlot();
		static void FreeTextureSlot(int index);
		static void FreeCBufferSlot(int index);

		template <typename T>
		static VertexBuffer<T>* CreateVertexBuffer()
		{
			VertexBuffer<T>* vertexBuffer = new VertexBuffer<T>();

			vertexBuffer->_pDevice = _pDevice;

			return vertexBuffer;
		}

		const static int TextureLimit = 64;
		const static int CBufferLimit = 8;

	private:
		ResourceFactory()
		{
		};

		static void _init(DX12Renderer* renderer, ID3D12DescriptorHeap* cbvSrvHeap);

		static int _cbufferIndex;
		static int _textureIndex;

		static std::vector<bool> _cbufferSlots;
		static std::vector<bool> _textureSlots;

		static ID3D12Device* _pDevice;
		static DX12Renderer* _pRenderer;
		static thread_local ID3D12CommandList* _pCommandList;
		static ID3D12DescriptorHeap* _pCbvSrvHeap;

		friend class DX12Renderer;
	};
}

