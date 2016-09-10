#include "../Rendering/DX12/IndexBufferInstance.h"
#include "../Rendering/DX12/VertexBufferInstance.h"
#include "../Rendering/DX12/ConstantBuffer.h"
#include "../Rendering/DX12/Material.h"
#include "../Rendering/DX12/Texture.h"
#include "../Rendering/DX12/DX12Renderer.h"
#include "../Rendering/DX12/GBuffer.h"

namespace Engine
{
	DX12Renderer* ResourceFactory::_pRenderer = nullptr;
	ID3D12Device* ResourceFactory::_pDevice = nullptr;
	ID3D12DescriptorHeap* ResourceFactory::_pCbvSrvHeap = nullptr;
	thread_local ID3D12CommandList* ResourceFactory::_pCommandList = nullptr;

	int ResourceFactory::_srvIndex = 0;
	int ResourceFactory::_cbvIndex = 0;
	int ResourceFactory::_srvCount = 0;
	int ResourceFactory::_cbvCount = 0;
	std::vector<bool> ResourceFactory::_srvSlots;
	std::vector<bool> ResourceFactory::_cbvSlots;

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
		VertexBufferInstance* instance = new VertexBufferInstance(vertexType);

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

	void ResourceFactory::SetObjectLimit(int limit)
	{
		if (_srvCount != 0)
		{
			Logging::LogError("Object limit has already been set.");
		}

		_srvCount = GBuffer::GBUFFER_NUM_BUFFERS + limit;
		_cbvCount = 1 + limit;

		_srvSlots = std::vector<bool>(_srvCount);
		_cbvSlots = std::vector<bool>(_cbvCount);

		for (int i = 0; i < _srvCount; ++i)
		{
			_srvSlots.push_back(false);
		}

		for (int i = 0; i < _cbvCount; ++i)
		{
			_cbvSlots.push_back(false);
		}
	}

	void ResourceFactory::_init(DX12Renderer* renderer, ID3D12DescriptorHeap* cbvSrvHeap)
	{
		_pRenderer = renderer;
		_pDevice = _pRenderer->_device.Get();
		_pCbvSrvHeap = cbvSrvHeap;
		_pCommandList = _pRenderer->_commandList.Get();
	}

	int ResourceFactory::GetTextureSlot()
	{
		for (int i = 0; i < _srvCount; ++i)
		{
			if (_srvSlots[i] == false)
			{
				_srvSlots[i] = true;
				return i + _cbvCount;
			}
		}

		Logging::LogError("Ran out of SRV slots. Consider raising the object limit.");

		return -1;
	}

	void ResourceFactory::FreeTextureSlot(int index)
	{
		_srvSlots[index - _cbvCount] = false;
	}

	int ResourceFactory::GetCBufferSlot()
	{
		for (int i = 0; i < _cbvCount; ++i)
		{
			if (_cbvSlots[i] == false)
			{
				_cbvSlots[i] = true;
				return i;
			}
		}

		Logging::LogError("Ran out of CBV slots. Consider raising the object limit.");

		return -1;
	}

	void ResourceFactory::FreeCBufferSlot(int index)
	{
		_cbvSlots[index] = false;
	}
}

