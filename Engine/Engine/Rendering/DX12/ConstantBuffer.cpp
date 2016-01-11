#include <DirectXMath.h>
#include <d3d12.h>
#include <vector>
#include "ConstantBuffer.h"
#include "HeapManager.h"
#include <memory>
#include "d3dx12.h"
#include "../../Factory/ResourceFactory.h"

namespace Engine
{
	ConstantBuffer::ConstantBuffer()
		: _pHeap(nullptr)
		, _descriptorSize(0)
		, _dirty(false)
		, _bufferSize(0)
	{
		_index = ResourceFactory::GetCBufferSlot();
		if (_index == -1)
		{
			Logging::LogError("Ran out of CBV slots. Consider increasing the CBufferLimit constant.");
		}
	}

	ConstantBuffer::~ConstantBuffer()
	{
		for (auto it = _cbuffer.begin(); it != _cbuffer.end(); ++it)
		{
			delete (*it).second.Data;
		}

		if (_index != -1)
		{
			ResourceFactory::FreeCBufferSlot(_index);
		}
	}

	void ConstantBuffer::SetFloat(std::string name, float value)
	{
		if (_cbuffer.find(name) == _cbuffer.end())
		{
			_bufferSize += sizeof(float);
			_cbuffer[name] = {new float(value), sizeof(float)};
		}
		else
		{
			*static_cast<float*>(_cbuffer[name].Data) = value;
		}

		if (!_dirty)
		{
			HeapTask(std::bind(&ConstantBuffer::UpdateBuffer, this));
			_dirty = true;
		}
	}

	void ConstantBuffer::SetInt(std::string name, int value)
	{
		if (_cbuffer.find(name) == _cbuffer.end())
		{
			_bufferSize += sizeof(int);
			_cbuffer[name] = {new int(value), sizeof(int)};
		}
		else
		{
			*static_cast<int*>(_cbuffer[name].Data) = value;
		}

		if (!_dirty)
		{
			HeapTask(std::bind(&ConstantBuffer::UpdateBuffer, this));
			_dirty = true;
		}
	}

	void ConstantBuffer::SetVector(std::string name, const DirectX::XMFLOAT4& value)
	{
		if (_cbuffer.find(name) == _cbuffer.end())
		{
			_bufferSize += sizeof(DirectX::XMFLOAT4);
			_cbuffer[name] = {new DirectX::XMFLOAT4(value), sizeof(DirectX::XMFLOAT4)};
		}
		else
		{
			*static_cast<DirectX::XMFLOAT4*>(_cbuffer[name].Data) = value;
		}

		if (!_dirty)
		{
			HeapTask(std::bind(&ConstantBuffer::UpdateBuffer, this));
			_dirty = true;
		}
	}

	void ConstantBuffer::SetMatrix(std::string name, const DirectX::XMFLOAT4X4& value)
	{
		if (_cbuffer.find(name) == _cbuffer.end())
		{
			_bufferSize += sizeof(DirectX::XMFLOAT4X4);
			_cbuffer[name] = {new DirectX::XMFLOAT4X4(value), sizeof(DirectX::XMFLOAT4X4)};
		}
		else
		{
			*static_cast<DirectX::XMFLOAT4X4*>(_cbuffer[name].Data) = value;
		}

		if (!_dirty)
		{
			HeapTask(std::bind(&ConstantBuffer::UpdateBuffer, this));
			_dirty = true;
		}
	}

	struct magic
	{
	};

	void ConstantBuffer::UpdateBuffer()
	{
		// Construct temporary buffer
		std::unique_ptr<char> data(new char[_bufferSize]);
		size_t offset = 0;
		for (auto it = _cbuffer.begin(); it != _cbuffer.end(); ++it)
		{
			DataItem item = it->second;
			memcpy(data.get() + offset, item.Data, item.Size);
			offset += item.Size;
		}

		// Allocate space and upload to the heap.
		_heapSize = _bufferSize;
		PrepareHeapResource();
		HeapManager::Upload(this, data.get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// Create the resource view.
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(_pHeap->GetCPUDescriptorHandleForHeapStart(), _index, _descriptorSize);
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = _pResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (_bufferSize + 255) & ~255;
		_pDevice->CreateConstantBufferView(&cbvDesc, cbvHandle);

		_dirty = false;
	}

	void ConstantBuffer::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		// Bind the constant buffer.
		if (_pResource != nullptr)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(_pHeap->GetGPUDescriptorHandleForHeapStart(), _index, _descriptorSize);
			commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
		}
	}
}

