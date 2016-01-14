#include "ResourceFactory.h"
#include "../Rendering/DX12/DX12Renderer.h"
#include "../Rendering/DX12/ConstantBuffer.h"
#include "../Rendering/DX12/IndexBuffer.h"
#include "../Rendering/DX12/Texture.h"
#include "../Rendering/DX12/Material.h"
#include "../Rendering/DX12/VertexBuffer.h"
#include <d3d12.h>

namespace Engine
{
	DX12Renderer* ResourceFactory::_pRenderer = nullptr;
	ID3D12Device* ResourceFactory::_pDevice = nullptr;
	ID3D12DescriptorHeap* ResourceFactory::_pCbvSrvHeap = nullptr;
	thread_local ID3D12CommandList* ResourceFactory::_pCommandList = nullptr;	

	int ResourceFactory::_cbufferIndex = 0;
	int ResourceFactory::_textureIndex = 0;
	std::vector<bool> ResourceFactory::_cbufferSlots = std::vector<bool>(CBufferLimit);
	std::vector<bool> ResourceFactory::_textureSlots = std::vector<bool>(TextureLimit);

	void ResourceFactory::AssignCommandList(ID3D12CommandList* commandList)
	{
		_pCommandList = commandList;
	}

	ID3D12CommandList* ResourceFactory::GetCommandList()
	{
		return _pCommandList;
	}

	IndexBufferInstance* ResourceFactory::CreateIndexBufferInstance()
	{
		IndexBufferInstance* instance = new IndexBufferInstance();

		instance->_pDevice = _pDevice;

		return instance;
	}

	VertexBufferInstance* ResourceFactory::CreateVertexBufferInstance(VertexType vertexType)
	{
		VertexBufferInstance* instance = new VertexBufferInstance();

		instance->_pDevice = _pDevice;

		return instance;
	}

	ConstantBuffer* ResourceFactory::CreateConstantBuffer()
	{
		ConstantBuffer* constantBuffer = new ConstantBuffer();

		constantBuffer->_pDevice = _pDevice;
		constantBuffer->_descriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		constantBuffer->_pHeap = _pCbvSrvHeap;

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
		texture->_pSrvHeap = _pCbvSrvHeap;

		return texture;
	}

	ID3D12DescriptorHeap* ResourceFactory::GetCbvSrvHeap()
	{
		return _pCbvSrvHeap;
	}

	void ResourceFactory::_init(DX12Renderer* renderer, ID3D12DescriptorHeap* cbvSrvHeap)
	{
		_pRenderer = renderer;
		_pDevice = _pRenderer->_device.Get();
		_pCbvSrvHeap = cbvSrvHeap;

		for (int i = 0; i < TextureLimit; ++i)
		{
			_textureSlots.push_back(false);
		}

		for (int i = 0; i < CBufferLimit; ++i)
		{
			_cbufferSlots.push_back(false);
		}
	}

	int ResourceFactory::GetTextureSlot()
	{
		for (int i = 0; i < TextureLimit; ++i)
		{
			if (_textureSlots[i] == false)
			{
				_textureSlots[i] = true;
				return i;
			}
		}

		return -1;
	}

	void ResourceFactory::FreeTextureSlot(int index)
	{
		_textureSlots[index] = false;
	}

	int ResourceFactory::GetCBufferSlot()
	{
		for (int i = 0; i < CBufferLimit; ++i)
		{
			if (_cbufferSlots[i] == false)
			{
				_cbufferSlots[i] = true;
				return int(i + TextureLimit);
			}
		}

		return -1;
	}

	void ResourceFactory::FreeCBufferSlot(int index)
	{
		_cbufferSlots[index - TextureLimit] = false;
	}
}

