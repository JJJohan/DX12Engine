#include <DirectXMath.h>
#include <d3d12.h>
#include <vector>
#include <memory>
#include "../../Utils/Logging.h"
#include "ConstantBuffer.h"
#include "HeapManager.h"
#include "VertexBuffer.h"

namespace Engine
{
	ConstantBuffer::ConstantBuffer()
		: _pDescriptor(nullptr)
	{
	}

	void ConstantBuffer::Build()
	{
		size_t size;
		if (!CheckBufferSize(&size))
		{
			return;
		}

		// Perform memory copy.
		size_t offset = 0;
		int index = 0;
		std::unique_ptr<char> memory(new char[size]);
		for (auto it = _instances.begin(); it != _instances.end(); ++it)
		{
			ConstantBufferInstance* instance = static_cast<ConstantBufferInstance*>(*it);
			if (instance->_index != index)
			{
				instance->SetIndex(index);
			}
			index++;

			const char* data = instance->GetData();
			memcpy(memory.get() + offset, data, CBUFFER_SLOT_SIZE);
			delete[] data;
			offset += CBUFFER_SLOT_SIZE;
		}

		// Allocate space and upload to the heap.
		_heapSize = size_t(offset);
		PrepareHeapResource();

		HeapManager::Upload(this, memory.get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// Create the resource view.
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = _pResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (_heapSize + 255) & ~255;
		_pDevice->CreateConstantBufferView(&cbvDesc, _pDescriptor->GetCPUDescriptorHandleForHeapStart());
	}

	void ConstantBuffer::Bind(ID3D12GraphicsCommandList* commandList)
	{
		// Bind the constant buffer.
		if (_pResource != nullptr)
		{
			commandList->SetGraphicsRootDescriptorTable(0, _pDescriptor->GetGPUDescriptorHandleForHeapStart());
		}
	}

	ConstantBufferInstance::ConstantBufferInstance(ID3D12DescriptorHeap* descriptorHeap)
		: _index(-1)
		, _slotUsage(0)
		, _pDescriptor(descriptorHeap)
		, _pVertexBuffer(nullptr)
	{
	}

	ConstantBufferInstance::~ConstantBufferInstance()
	{
		for (auto it = _cbuffer.begin(); it != _cbuffer.end(); ++it)
		{
			delete (*it).second.Data;
		}
	}

	size_t ConstantBufferInstance::GetSize() const
	{
		return size_t(CBUFFER_SLOT_SIZE);
	}

	void ConstantBufferInstance::SetIndex(int index)
	{
		_index = index;

		if (_pVertexBuffer != nullptr)
		{
			_pVertexBuffer->SetBufferIndex(index);
		}
	}

	void ConstantBufferInstance::SetVertexBuffer(VertexBufferInstance* vertexBuffer)
	{
		_pVertexBuffer = vertexBuffer;
	}

	int ConstantBufferInstance::GetIndex() const
	{
		return _index;
	}

	void ConstantBufferInstance::AssignBuffer()
	{
		if (_pBuffer == nullptr)
		{
			ConstantBuffer* buffer = BufferBucket::PrepareBuffer<ConstantBuffer>(this);
			buffer->_pDescriptor = _pDescriptor;
			_pBuffer = buffer;
		}
		else
		{
			ConstantBuffer* buffer = static_cast<ConstantBuffer*>(_pBuffer);
			buffer->RequestBuild();
		}
	}

	void ConstantBufferInstance::SetFloat(std::string name, float value)
	{
		if (_cbuffer.find(name) == _cbuffer.end())
		{
			if (_slotUsage + sizeof(float) > CBUFFER_SLOT_SIZE)
			{
				Logging::LogError("Variable '{0}' has exceeded slot size.", name);
				return;
			}

			_slotUsage += sizeof(float);
			_cbuffer[name] = { new float(value), sizeof(float) };
		}
		else
		{
			*static_cast<float*>(_cbuffer[name].Data) = value;
		}

		AssignBuffer();
	}

	void ConstantBufferInstance::SetInt(std::string name, int value)
	{
		if (_cbuffer.find(name) == _cbuffer.end())
		{
			if (_slotUsage + sizeof(int) > CBUFFER_SLOT_SIZE)
			{
				Logging::LogError("Variable '{0}' has exceeded slot size.", name);
				return;
			}

			_slotUsage += sizeof(int);
			_cbuffer[name] = { new int(value), sizeof(int) };
		}
		else
		{
			*static_cast<int*>(_cbuffer[name].Data) = value;
		}

		AssignBuffer();
	}

	void ConstantBufferInstance::SetVector(std::string name, const DirectX::XMFLOAT4& value)
	{
		if (_cbuffer.find(name) == _cbuffer.end())
		{
			if (_slotUsage + sizeof(DirectX::XMFLOAT4) > CBUFFER_SLOT_SIZE)
			{
				Logging::LogError("Variable '{0}' has exceeded slot size.", name);
				return;
			}

			_slotUsage += sizeof(DirectX::XMFLOAT4);
			_cbuffer[name] = { new DirectX::XMFLOAT4(value), sizeof(DirectX::XMFLOAT4) };
		}
		else
		{
			*static_cast<DirectX::XMFLOAT4*>(_cbuffer[name].Data) = value;
		}

		AssignBuffer();
	}

	void ConstantBufferInstance::SetMatrix(std::string name, const DirectX::XMFLOAT4X4& value)
	{
		if (_cbuffer.find(name) == _cbuffer.end())
		{
			if (_slotUsage + sizeof(DirectX::XMFLOAT4X4) > CBUFFER_SLOT_SIZE)
			{
				Logging::LogError("Variable '{0}' has exceeded slot size.", name);
				return;
			}

			_slotUsage += sizeof(DirectX::XMFLOAT4X4);
			_cbuffer[name] = { new DirectX::XMFLOAT4X4(value), sizeof(DirectX::XMFLOAT4X4) };
		}
		else
		{
			*static_cast<DirectX::XMFLOAT4X4*>(_cbuffer[name].Data) = value;
		}

		AssignBuffer();
	}

	const char* ConstantBufferInstance::GetData() const
	{
		// Construct temporary buffer
		char* data = new char[CBUFFER_SLOT_SIZE];
		ZeroMemory(data, CBUFFER_SLOT_SIZE);
		size_t offset = 0;
		for (auto it = _cbuffer.begin(); it != _cbuffer.end(); ++it)
		{
			DataItem item = it->second;
			memcpy(data + offset, item.Data, item.Size);
			offset += item.Size;
		}

		return data;
	}
}

