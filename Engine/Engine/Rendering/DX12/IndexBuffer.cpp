#include "IndexBuffer.h"
#include "../../Utils/Logging.h"

namespace Engine
{
	IndexBuffer::IndexBuffer()
		: _indexCount(0)
		, _indexBufferView()
	{
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	void IndexBuffer::SetIndices(std::vector<int> indices)
	{
		if (!_indices.empty())
		{
			Logging::LogError("Re-using index buffers is not implemented.");
		}

		_indices = indices;
		_indexCount = _indices.size();
		const UINT indexBufferSize = UINT(sizeof(int) * _indexCount);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		LOGFAILEDCOM(
			_pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&_pResource)));

		UINT8* pVertexDataBegin;
		LOGFAILEDCOM(
			_pResource->Map(0, nullptr, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, &_indices[0], indexBufferSize);
		_pResource->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		_indexBufferView.BufferLocation = _pResource->GetGPUVirtualAddress();
		_indexBufferView.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_indexBufferView.SizeInBytes = indexBufferSize;
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

