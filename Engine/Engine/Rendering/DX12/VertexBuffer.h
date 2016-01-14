#pragma once

#include <vector>
#include <unordered_set>
#include <d3d12.h>
#include <DirectXMath.h>
#include "HeapResource.h"
#include <set>

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
		Vertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 col, DirectX::XMFLOAT2 uv)
		{
			Pos = pos;
			Col = col;
			Uv = uv;
		}

		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Col;
		DirectX::XMFLOAT2 Uv;
	};

	class VertexBufferInstance;

	class VertexBuffer : protected HeapResource
	{
	public:
		~VertexBuffer();

		void Bind(ID3D12GraphicsCommandList* commandList) const;

	private:
		VertexBuffer();
		void RequestBuild();
		void Build();
		void ReleaseInstance(VertexBufferInstance* instance);

		static VertexBuffer* PrepareBuffer(VertexBufferInstance* instance, ID3D12Device* device);
		
		static std::unordered_set<VertexBuffer*> _vertexBuffers;
		std::set<VertexBufferInstance*> _instances;

		size_t _totalSize;
		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

		friend class VertexBufferInstance;
	};

	class VertexBufferInstance
	{
	public:
		~VertexBufferInstance();

		void Bind(ID3D12GraphicsCommandList* commandList) const;

		void SetVertices(std::vector<Vertex> vertices);
		std::vector<Vertex> GetVertices() const;

		const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const;
		size_t Count() const;
		static size_t VertexSize();

	private:
		VertexBufferInstance(VertexType vertexType);

		ID3D12Device* _pDevice;
		std::vector<Vertex> _vertices;
		VertexBuffer* _pBuffer;
		int _offset;
		int _size;
		std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;

		friend class ResourceFactory;
	};
}

