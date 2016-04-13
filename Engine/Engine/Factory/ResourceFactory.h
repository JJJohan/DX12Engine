#pragma once

struct ID3D12CommandList;
struct ID3D12DescriptorHeap;
struct ID3D12Device;

namespace Engine
{
	class VertexBufferInstance;
	class IndexBufferInstance;
	class ConstantBufferInstance;
	class DX12Renderer;
	class Texture;
	class Material;

	enum VertexType;

	class ResourceFactory
	{
	public:
		static void AssignCommandList(ID3D12CommandList* commandList);
		static ID3D12CommandList* GetCommandList();
		static ENGINE_API ConstantBufferInstance* CreateConstantBuffer();
		static ENGINE_API VertexBufferInstance* CreateVertexBufferInstance();
		static ENGINE_API IndexBufferInstance* CreateIndexBufferInstance();
		static ENGINE_API Material* CreateMaterial();
		static ENGINE_API Texture* CreateTexture(int width, int height);

		static int GetTextureSlot();
		static void FreeTextureSlot(int index);

		static const int TextureLimit = 32;
		static const int CBufferLimit = 8;

	private:
		ResourceFactory() { };

		static void _init(DX12Renderer* renderer, ID3D12DescriptorHeap* cbvSrvHeap);

		static int _textureIndex;

		static std::vector<bool> _textureSlots;

		static ID3D12Device* _pDevice;
		static DX12Renderer* _pRenderer;
		static thread_local ID3D12CommandList* _pCommandList;
		static ID3D12DescriptorHeap* _pCbvSrvHeap;

		friend class DX12Renderer;
	};
}

