#include <DirectXMath.h>
#include <d3d12.h>
#include <vector>
#include "ConstantBuffer.h"
#include "HeapManager.h"
#include <memory>

namespace Engine
{
	ConstantBuffer::ConstantBuffer()
		: _dirty(false)
		  , _bufferSize(0)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
		for (auto it = _cbuffer.begin(); it != _cbuffer.end(); ++it)
		{
			delete (*it).second.Data;
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

		_dirty = false;
	}

	void ConstantBuffer::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		// Bind the constant buffer.
		if (_pResource != nullptr)
		{
			commandList->SetGraphicsRootConstantBufferView(0, _pResource->GetGPUVirtualAddress());
		}
	}
}

