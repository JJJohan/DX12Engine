#include "VertexBufferInstance.h"
#include "VertexBuffer.h"
#include "Material.h"

namespace Engine
{
	VertexBufferInstance::VertexBufferInstance(VertexType vertexType)
		: _size(0)
		, _bufferIndex(-1)
	{
		switch (vertexType)
		{
			case VERTEX_POS_COL_UV:
				_inputLayout = Material::Default_Input_Layout;
				return;

			case VERTEX_PBR:
				_inputLayout = Material::PBR_Input_Layout;
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

