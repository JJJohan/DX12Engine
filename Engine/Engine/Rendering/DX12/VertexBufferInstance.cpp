#include "VertexBufferInstance.h"
#include "VertexBuffer.h"

namespace Engine
{
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
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			_inputLayout = inputElementDescs;
			return;

		case VERTEX_POS_COL_UV:
			inputElementDescs =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			_inputLayout = inputElementDescs;
			return;

		case VERTEX_POS_UV:
			inputElementDescs =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			_inputLayout = inputElementDescs;
			return;

		default:
			Logging::LogError("Unknown vertex layout.");
			break;
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
}