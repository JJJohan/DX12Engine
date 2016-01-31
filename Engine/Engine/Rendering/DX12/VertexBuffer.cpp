#include "VertexBuffer.h"
#include "VertexBufferInstance.h"
#include "HeapManager.h"

namespace Engine
{
	VertexBuffer::VertexBuffer()
		: _vertexBufferView()
	{
	}

	void VertexBuffer::Bind(ID3D12GraphicsCommandList* commandList)
	{
		if (!_bound)
		{
			commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			_bound = true;
		}
	}

	void VertexBuffer::Build()
	{
		size_t size;
		if (!CheckBufferSize(&size))
		{
			return;
		}

		bool sameSize = (size == _heapSize);

		// Perform memory copy.
		size_t offset = 0;
		size_t totalVertices = 0;
		std::unique_ptr<char> memory(new char[size]);
		for (auto it = _instances.begin(); it != _instances.end(); ++it)
		{
			VertexBufferInstance* instance = static_cast<VertexBufferInstance*>(*it);
			std::vector<Vertex> vertices = instance->GetVertices();
			size_t copySize = instance->GetSize();

			instance->SetOffset(totalVertices);
			totalVertices += instance->Count();
			memcpy(memory.get() + offset, &vertices[0], copySize);
			offset += copySize;
		}

		_heapSize = size_t(offset);
		PrepareHeapResource();

		HeapManager::Upload(this, memory.get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// Initialize the vertex buffer view.
		if (!sameSize)
		{
			_vertexBufferView = {};
			_vertexBufferView.BufferLocation = _pResource->GetGPUVirtualAddress();
			_vertexBufferView.StrideInBytes = sizeof(Vertex);
			_vertexBufferView.SizeInBytes = UINT(_heapSize);
		}
	}
}

