#include "ResourceFactory.h"
#include "../Rendering/DX12/DX12Renderer.h"
#include "../Rendering/DX12/ConstantBuffer.h"
#include "../Rendering/DX12/IndexBuffer.h"
#include "../Rendering/DX12/Texture.h"
#include "../Rendering/DX12/Material.h"

namespace Engine
{
	DX12Renderer* ResourceFactory::_pRenderer = nullptr;
	ID3D12Device* ResourceFactory::_pDevice = nullptr;
	thread_local ID3D12CommandList* ResourceFactory::_pCommandList = nullptr;

	void ResourceFactory::AssignCommandList(ID3D12CommandList* commandList)
	{
		_pCommandList = commandList;
	}

	ID3D12CommandList* ResourceFactory::GetCommandList()
	{
		return _pCommandList;
	}

	IndexBuffer* ResourceFactory::CreateIndexBuffer()
	{
		IndexBuffer* indexBuffer = new IndexBuffer();

		indexBuffer->_pDevice = _pDevice;

		return indexBuffer;
	}

	ConstantBuffer* ResourceFactory::CreateConstantBuffer()
	{
		ConstantBuffer* constantBuffer = new ConstantBuffer();

		constantBuffer->_pDevice = _pDevice;

		return constantBuffer;
	}

	Material* ResourceFactory::CreateMaterial()
	{
		Material* material = new Material();

		material->_pDevice = _pDevice;
		material->_pRootSignature = _pRenderer->_rootSignature.Get();

		return material;
	}

	Texture* ResourceFactory::CreateTexture()
	{
		Texture* texture = new Texture();

		texture->_pDevice = _pDevice;
		texture->_pSrvHeap = _pRenderer->_srvHeap.Get();

		return texture;
	}

	void ResourceFactory::_init(DX12Renderer* renderer)
	{
		_pRenderer = renderer;
		_pDevice = _pRenderer->_device.Get();
	}
}

