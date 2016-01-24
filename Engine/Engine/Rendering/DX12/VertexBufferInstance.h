#pragma once

#include "BufferInstance.h"
#include "../../Data/Vector2.h"
#include "../../Data/Vector3.h"
#include "../../Data/Vector4.h"
#include "../../Data/Colour.h"

namespace Engine
{
	struct Vertex
	{
		Vertex()
		{
		}

		Vertex(Vector3 pos, Colour col, Vector2 uv)
		{
			Pos = pos;
			Col = col;
			Uv = Vector3(uv.X, uv.Y, 0.0f);
		}

		Vector3 Pos;
		Colour Col;
		Vector3 Uv;
	};

	enum VertexType
	{
		VERTEX_POS_COL,
		VERTEX_POS_COL_UV,
		VERTEX_POS_UV
	};

	class VertexBufferInstance : public BufferInstance
	{
	public:
		ENGINE_API VertexBufferInstance();

		ENGINE_API void SetVertices(std::vector<Vertex> vertices);
		ENGINE_API std::vector<Vertex> GetVertices() const;

		ENGINE_API const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const;
		size_t Count() const;
		size_t GetSize() const override;

		void SetBufferIndex(int index);
		int GetBufferIndex() const;

		static size_t VertexSize();

	private:
		std::vector<Vertex> _vertices;

		int _size;
		int _bufferIndex;
		std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;

		friend class VertexBuffer;
	};
}