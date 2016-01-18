#include "IndexBuffer.h"
#include "HeapManager.h"
#include "../../Utils/Logging.h"

namespace Engine
{
	IndexBuffer::IndexBuffer()
		: _indexBufferView()
	{
	}

	void IndexBuffer::Build()
	{
		size_t size;
		if (!CheckBufferSize(&size))
		{
			return;
		}

		// Perform memory copy.
		size_t offset = 0;
		size_t totalIndices = 0;
		std::unique_ptr<char> memory(new char[size]);
		for (auto it = _instances.begin(); it != _instances.end(); ++it)
		{
			IndexBufferInstance* instance = static_cast<IndexBufferInstance*>(*it);
			std::vector<int> indices = instance->GetIndices();
			size_t copySize = instance->GetSize();

			instance->SetOffset(totalIndices);
			totalIndices += instance->Count();
			memcpy(memory.get() + offset, &indices[0], copySize);
			offset += copySize;
		}

		_heapSize = size_t(offset);
		PrepareHeapResource();

		HeapManager::Upload(this, memory.get(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

		// Initialize the index buffer view.
		_indexBufferView.BufferLocation = _pResource->GetGPUVirtualAddress();
		_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		_indexBufferView.SizeInBytes = UINT(_heapSize);
	}

	void IndexBufferInstance::SetIndices(std::vector<int> indices)
	{
		_indices = indices;

		if (_pBuffer == nullptr)
		{
			_pBuffer = BufferBucket::PrepareBuffer<IndexBuffer>(this);
		}
		else
		{
			IndexBuffer* buffer = static_cast<IndexBuffer*>(_pBuffer);
			buffer->RequestBuild();
		}
	}

	std::vector<int> IndexBufferInstance::GetIndices() const
	{
		return _indices;
	}

	size_t IndexBufferInstance::Count() const
	{
		return _indices.size();
	}

	size_t IndexBufferInstance::GetSize() const
	{
		return Count() * sizeof(int);
	}

	void IndexBuffer::Bind(ID3D12GraphicsCommandList* commandList)
	{
		if (!_bound)
		{
			commandList->IASetIndexBuffer(&_indexBufferView);
			_bound = true;
		}
	}
}

