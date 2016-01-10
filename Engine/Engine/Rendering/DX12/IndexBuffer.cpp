#include "IndexBuffer.h"
#include "HeapManager.h"

namespace Engine
{
	IndexBuffer::IndexBuffer()
		: _indexCount(0)
		  , _indexBufferView()
	{
	}

	void IndexBuffer::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		commandList->IASetIndexBuffer(&_indexBufferView);
	}

	void IndexBuffer::SetIndices(std::vector<int> indices)
	{
		_indices = indices;
		_indexCount = _indices.size();

		HeapTask(std::bind(&IndexBuffer::CreateIndexBuffer, this));
	}

	void IndexBuffer::CreateIndexBuffer()
	{
		_heapSize = UINT(sizeof(int) * _indexCount);
		PrepareHeapResource();

		HeapManager::Upload(this, &_indices[0], D3D12_RESOURCE_STATE_INDEX_BUFFER);

		// Initialize the index buffer view.
		_indexBufferView.BufferLocation = _pResource->GetGPUVirtualAddress();
		_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		_indexBufferView.SizeInBytes = UINT(_heapSize);
	}

	std::vector<int> IndexBuffer::GetIndices() const
	{
		return _indices;
	}

	int IndexBuffer::Count() const
	{
		return int(_indexCount);
	}
}

