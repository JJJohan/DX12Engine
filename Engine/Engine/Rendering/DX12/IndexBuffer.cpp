#include "IndexBuffer.h"
#include "IndexBufferInstance.h"
#include "HeapManager.h"

namespace Engine
{
	IndexBuffer::IndexBuffer()
		: _indexBufferView() { }

	void IndexBuffer::Build()
	{
		size_t size;
		if (!CheckBufferSize(&size))
		{
			return;
		}

		bool sameSize = (size == _heapSize);

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
		PrepareHeapResource(D3D12_RESOURCE_STATE_INDEX_BUFFER);

		HeapManager::Upload(this, memory.get());

		// Initialize the index buffer view.
		if (!sameSize)
		{
			_indexBufferView = {};
			_indexBufferView.BufferLocation = _pResource->GetGPUVirtualAddress();
			_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
			_indexBufferView.SizeInBytes = UINT(_heapSize);
		}
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

