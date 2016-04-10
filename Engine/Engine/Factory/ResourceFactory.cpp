#include "../Rendering/DX12/IndexBufferInstance.h"
#include "../Rendering/DX12/VertexBufferInstance.h"
#include "../Rendering/DX12/ConstantBuffer.h"
#include "../Rendering/DX12/Material.h"
#include "../Rendering/DX12/Texture.h"
#include "../Rendering/DX12/DX12Renderer.h"

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

	VertexBufferInstance* ResourceFactory::CreateVertexBufferInstance()
	{
		VertexBufferInstance* instance = new VertexBufferInstance();

		instance->_pDevice = _pDevice;

		return instance;
	}

	ConstantBufferInstance* ResourceFactory::CreateConstantBuffer()
	{
		ConstantBufferInstance* constantBuffer = new ConstantBufferInstance(_pCbvSrvHeap);

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

	Texture* ResourceFactory::CreateTexture(int width, int height)
	{
		Texture* texture = new Texture(width, height);

		texture->_pDevice = _pDevice;
		texture->_pSrvHeap = _pCbvSrvHeap;

		return texture;
	}

	void ResourceFactory::_init(DX12Renderer* renderer, ID3D12DescriptorHeap* cbvSrvHeap)
	{
		_pRenderer = renderer;
		_pDevice = _pRenderer->_device.Get();
		_pCbvSrvHeap = cbvSrvHeap;
		_pCommandList = _pRenderer->_commandList.Get();

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
				return i + CBufferLimit;
			}
		}

		return -1;
	}

	void ResourceFactory::FreeTextureSlot(int index)
	{
		_textureSlots[index - CBufferLimit] = false;
	}

	int ResourceFactory::GetCBufferSlot()
	{
		for (int i = 0; i < CBufferLimit; ++i)
		{
			if (_cbufferSlots[i] == false)
			{
				_cbufferSlots[i] = true;
				return i;
			}
		}

		return -1;
	}

	void ResourceFactory::FreeCBufferSlot(int index)
	{
		_cbufferSlots[index] = false;
	}
}

