#include "VertexBuffer.h"
#include "../../Utils/Logging.h"
#include "HeapManager.h"

namespace Engine
{
	VertexBuffer::VertexBuffer()
		: _vertexBufferView()
	{
	}

	VertexBufferInstance::VertexBufferInstance()
		: _size(0)
		  , _bufferIndex(-1)
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;

		VertexType vertexType = VERTEX_POS_COL_UV;
		switch (vertexType)
		{
		case VERTEX_POS_COL:
			inputElementDescs =
				{
					{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
					{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				};

			_inputLayout = inputElementDescs;
			return;

		case VERTEX_POS_COL_UV:
			inputElementDescs =
				{
					{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
					{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
					{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				};

			_inputLayout = inputElementDescs;
			return;

		case VERTEX_POS_UV:
			inputElementDescs =
				{
					{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
					{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				};

			_inputLayout = inputElementDescs;
			return;

		default:
			Logging::LogError("Unknown vertex layout.");
			break;
		}
	}

	void VertexBuffer::Bind(ID3D12GraphicsCommandList* commandList)
	{
		if (!_bound)
		{
			commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			_bound = true;
		}	
	}

	const std::vector<D3D12_INPUT_ELEMENT_DESC>& VertexBufferInstance::GetInputLayout() const
	{
		return _inputLayout;
	}

	size_t VertexBufferInstance::VertexSize()
	{
		return sizeof(Vertex);
	}

	size_t VertexBufferInstance::Count() const
	{
		return _vertices.size();
	}

	void VertexBufferInstance::SetBufferIndex(int index)
	{
		if (_bufferIndex != index)
		{
			_bufferIndex = index;

			size_t count = Count();
			for (int i = 0; i < count; ++i)
			{
				_vertices[i].Uv.z = float(index);
			}

			VertexBuffer* buffer = static_cast<VertexBuffer*>(_pBuffer);
			buffer->RequestBuild();
		}
	}

	int VertexBufferInstance::GetBufferIndex() const
	{
		return _bufferIndex;
	}

	size_t VertexBufferInstance::GetSize() const
	{
		return Count() * VertexSize();
	}

	void VertexBufferInstance::SetVertices(std::vector<Vertex> vertices)
	{
		_vertices = vertices;
		size_t count = Count();
		if (_bufferIndex != -1)
		{
			for (int i = 0; i < count; ++i)
			{
				_vertices[i].Uv.z = float(_bufferIndex);
			}
		}

		if (_pBuffer == nullptr)
		{
			_pBuffer = BufferBucket::PrepareBuffer<VertexBuffer>(this);
		}
		else
		{
			VertexBuffer* buffer = static_cast<VertexBuffer*>(_pBuffer);
			buffer->RequestBuild();
		}
	}

	std::vector<Vertex> VertexBufferInstance::GetVertices() const
	{
		return _vertices;
	}

	void VertexBuffer::Build()
	{
		size_t size;
		if (!CheckBufferSize(&size))
		{
			return;
		}

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
		_vertexBufferView = {};
		_vertexBufferView.BufferLocation = _pResource->GetGPUVirtualAddress();
		_vertexBufferView.StrideInBytes = sizeof(Vertex);
		_vertexBufferView.SizeInBytes = UINT(_heapSize);
	}
}

