#pragma once

#include "BufferInstance.h"

namespace DirectX
{
	struct XMFLOAT3;
	struct XMFLOAT4;
}

namespace Engine
{
	struct Vertex
	{
		Vertex() { }

		Vertex(const Vector3& pos, const Colour& col, const Vector2& uv)
			: Pos(pos)
			, Col(col)
			, Uv(uv.X, uv.Y, 0.0f) { }

		Vertex(const Vector3& pos, const Colour& col, const Vector2& uv, const Vector3& normal)
			: Pos(pos)
			, Col(col)
			, Uv(uv.X, uv.Y, 0.0f)
			, Normal(normal.X, normal.Y, normal.Z) { }

		Vertex(const Vector3& pos, const Colour& col, const Vector2& uv, const Vector3& normal, const float& roughness, const float& metallic)
			: Pos(pos)
			, Col(col)
			, Uv(uv.X, uv.Y, 0.0f)
			, Normal(normal.X, normal.Y, normal.Z)
			, Roughness_Metallic(roughness, metallic) { }

		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Col;
		DirectX::XMFLOAT3 Uv;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 Roughness_Metallic;
	};

	enum VertexType
	{
		VERTEX_POS_COL_UV,
		VERTEX_PBR
	};

	class VertexBufferInstance : public BufferInstance
	{
	public:
		ENGINE_API VertexBufferInstance(VertexType vertexType);

		ENGINE_API void SetVertices(std::vector<Vertex> vertices);
		ENGINE_API std::vector<Vertex> GetVertices() const;

		ENGINE_API const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const;
		ENGINE_API size_t Count() const override;
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

