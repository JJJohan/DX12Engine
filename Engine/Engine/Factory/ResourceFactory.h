#pragma once
#include "Rendering/DX12/VertexBufferInstance.h"

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
	
	class ResourceFactory
	{
	public:
		static void AssignCommandList(ID3D12CommandList* commandList);
		static ID3D12CommandList* GetCommandList();
		static ENGINE_API ConstantBufferInstance* CreateConstantBuffer();
		static ENGINE_API VertexBufferInstance* CreateVertexBufferInstance(VertexType vertexType = VERTEX_PBR);
		static ENGINE_API IndexBufferInstance* CreateIndexBufferInstance();
		static ENGINE_API Material* CreateMaterial();
		static ENGINE_API Texture* CreateTexture(int width, int height);
		static void SetObjectLimit(int limit);

		static int GetTextureSlot();
		static void FreeTextureSlot(int index);

		static int GetCBufferSlot();
		static void FreeCBufferSlot(int index);

		static int SRVs() { return _srvCount; }
		static int CBVs() { return _cbvCount; }

	private:
		ResourceFactory() { };

		static void _init(DX12Renderer* renderer, ID3D12DescriptorHeap* cbvSrvHeap);

		static int _srvIndex;
		static int _cbvIndex;
		static int _srvCount;
		static int _cbvCount;

		static std::vector<bool> _srvSlots;
		static std::vector<bool> _cbvSlots;

		static ID3D12Device* _pDevice;
		static DX12Renderer* _pRenderer;
		static thread_local ID3D12CommandList* _pCommandList;
		static ID3D12DescriptorHeap* _pCbvSrvHeap;

		friend class DX12Renderer;
	};
}

