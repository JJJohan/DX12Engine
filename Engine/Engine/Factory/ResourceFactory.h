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

		template <typename T>
		static VertexBuffer<T>* CreateVertexBuffer()
		{
			VertexBuffer<T>* vertexBuffer = new VertexBuffer<T>();

			vertexBuffer->_pDevice = _pDevice;

			return vertexBuffer;
		}

	private:
		ResourceFactory()
		{
		};

		static void _init(DX12Renderer* renderer);

		static ID3D12Device* _pDevice;
		static DX12Renderer* _pRenderer;
		static thread_local ID3D12CommandList* _pCommandList;

		friend class DX12Renderer;
	};
}

