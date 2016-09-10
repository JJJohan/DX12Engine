#include "ConstantBuffer.h"
#include "HeapManager.h"
#include "VertexBufferInstance.h"

namespace Engine
{
	ConstantBuffer::ConstantBuffer()
		: _pDescriptor(nullptr)
		, _pCopyBuffer(nullptr)
		, _copyBufferSize(0) { }

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
		bool sameSize = (((size + 255) & ~255) == _heapSize);

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
		
		size_t packed = size_t((offset + 255) & ~255);
		if (_heapSize < packed)
		{
			// Heap needs to be recreated to enlarge the CBV.
			HeapManager::ReleaseHeap(this);
		}

		// Allocate space and upload to the heap.
		_heapSize = packed;
		MarkDynamic();

		HeapManager::Upload(this, _pCopyBuffer);

		// Create the resource view.
		if (!sameSize)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = _pResource->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = UINT(_heapSize);
			_pDevice->CreateConstantBufferView(&cbvDesc, _pDescriptor->GetCPUDescriptorHandleForHeapStart());
		}
	}

	void ConstantBuffer::Bind(ID3D12GraphicsCommandList* commandList)
	{
		if (!_bound)
		{
			// Bind the constant buffer.
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(_pDescriptor->GetGPUDescriptorHandleForHeapStart(), 0, D3DUtils::GetSRVDescriptorSize());
			commandList->SetGraphicsRootDescriptorTable(_rootSlot, cbvHandle);

			_bound = true;
		}
	}

	ConstantBufferInstance::ConstantBufferInstance(ID3D12DescriptorHeap* descriptorHeap)
		: _slotUsage(0)
		, _pDescriptor(descriptorHeap)
		, _pVertexBuffer(nullptr)
		, _rootSlot(0)
	{
		_index = ResourceFactory::GetCBufferSlot();
	}

	ConstantBufferInstance::~ConstantBufferInstance()
	{
		if (_index != -1)
		{
			ResourceFactory::FreeCBufferSlot(_index);
		}

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

	size_t ConstantBufferInstance::Count() const
	{
		return _cbuffer.size();
	}
}

