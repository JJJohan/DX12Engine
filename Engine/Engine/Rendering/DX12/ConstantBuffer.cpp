#include "ConstantBuffer.h"
#include "HeapManager.h"
#include "VertexBuffer.h"

namespace Engine
{
	ConstantBuffer::ConstantBuffer()
		: _pDescriptor(nullptr)
		  , _pCopyBuffer(nullptr)
		  , _copyBufferSize(0)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
		delete[] _pCopyBuffer;
	}

	void ConstantBuffer::Build()
	{
		size_t size;
		if (!CheckBufferSize(&size))
		{
			return;
		}

		if (_pCopyBuffer == nullptr || _copyBufferSize < size)
		{
			delete[] _pCopyBuffer;
			_pCopyBuffer = new char[size];
			_copyBufferSize = size;
		}
		ZeroMemory(_pCopyBuffer, _copyBufferSize);
		bool sameSize = (size == _heapSize);

		// Perform memory copy.
		size_t offset = 0;
		int index = 0;
		for (auto it = _instances.begin(); it != _instances.end(); ++it)
		{
			ConstantBufferInstance* instance = static_cast<ConstantBufferInstance*>(*it);
			if (instance->_index != index)
			{
				instance->SetIndex(index);
			}
			index++;

			std::vector<ConstantBufferInstance::DataItem> data = instance->GetData();
			for (int i = 0; i < data.size(); ++i)
			{
				memcpy(_pCopyBuffer + offset, data[i].Data, data[i].Size);
			}
			offset += CBUFFER_SLOT_SIZE;
		}

		// Allocate space and upload to the heap.
		_heapSize = size_t(offset);
		MarkDynamic();

		HeapManager::Upload(this, _pCopyBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// Create the resource view.
		if (!sameSize)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = _pResource->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = (_heapSize + 255) & ~255;
			_pDevice->CreateConstantBufferView(&cbvDesc, _pDescriptor->GetCPUDescriptorHandleForHeapStart());
		}
	}

	void ConstantBuffer::Bind(ID3D12GraphicsCommandList* commandList)
	{
		if (!_bound)
		{
			// Bind the constant buffer.
			commandList->SetGraphicsRootDescriptorTable(0, _pDescriptor->GetGPUDescriptorHandleForHeapStart());
			_bound = true;
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
			_cbuffer[name] = {new float(value), sizeof(float)};
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
			_cbuffer[name] = {new int(value), sizeof(int)};
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
			_cbuffer[name] = {new DirectX::XMFLOAT4(value), sizeof(DirectX::XMFLOAT4)};
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
			_cbuffer[name] = {new DirectX::XMFLOAT4X4(value), sizeof(DirectX::XMFLOAT4X4)};
		}
		else
		{
			*static_cast<DirectX::XMFLOAT4X4*>(_cbuffer[name].Data) = value;
		}

		AssignBuffer();
	}

	std::vector<ConstantBufferInstance::DataItem> ConstantBufferInstance::GetData() const
	{
		// Construct temporary buffer
		std::vector<DataItem> data;
		for (auto it = _cbuffer.begin(); it != _cbuffer.end(); ++it)
		{
			DataItem item = it->second;
			data.push_back({item.Data, item.Size});
		}

		return data;
	}
}

