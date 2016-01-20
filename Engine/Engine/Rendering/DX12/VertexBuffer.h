#pragma once

#include "BufferBucket.h"
#include "BufferInstance.h"

namespace Engine
{
	enum VertexType
	{
		VERTEX_POS_COL,
		VERTEX_POS_COL_UV,
		VERTEX_POS_UV
	};

	struct Vertex
	{
		Vertex()
		{
		}

		Vertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 col, DirectX::XMFLOAT2 uv)
		{
			Pos = pos;
			Col = col;
			Uv = DirectX::XMFLOAT3(uv.x, uv.y, 0.0f);
		}

		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Col;
		DirectX::XMFLOAT3 Uv;
	};

	class VertexBufferInstance;

	class VertexBuffer : public BufferBucket
	{
	public:
		VertexBuffer();

	private:
		void Build() override;
		void Bind(ID3D12GraphicsCommandList* commandList) override;

		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

		friend class VertexBufferInstance;
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

