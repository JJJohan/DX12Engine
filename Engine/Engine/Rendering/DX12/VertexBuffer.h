#pragma once

#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
#include "HeapResource.h"
#include "d3dx12.h"
#include "../../Utils/Logging.h"
#include "DX12Renderer.h"
#include "CommandQueue.h"
#include "HeapManager.h"
#include "../../Factory/Factory.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Engine
{
	struct VertexPosCol
	{
		VertexPosCol(XMFLOAT3 pos, XMFLOAT4 col)
		{
			Pos = pos;
			Col = col;
		}

		XMFLOAT3 Pos;
		XMFLOAT4 Col;
	};

	struct VertexPosColUv
	{
		VertexPosColUv(XMFLOAT3 pos, XMFLOAT4 col, XMFLOAT2 uv)
		{
			Pos = pos;
			Col = col;
			Uv = uv;
		}

		XMFLOAT3 Pos;
		XMFLOAT4 Col;
		XMFLOAT2 Uv;
	};

	struct VertexPosUv
	{
		VertexPosUv(XMFLOAT3 pos, XMFLOAT2 uv)
		{
			Pos = pos;
			Uv = uv;
		}

		XMFLOAT3 Pos;
		XMFLOAT2 Uv;
	};

	class VertexBufferBase : protected HeapResource
	{
	public:
		void Bind(ID3D12GraphicsCommandList* commandList) const;
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const;
		int Count() const;

	protected:
		VertexBufferBase();

		size_t _vertexCount;
		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
		std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
	};

	template <typename T>
	class VertexBuffer : public VertexBufferBase
	{
	public:
		void SetVertices(std::vector<T> vertices)
		{
			_vertices = vertices;
			_vertexCount = _vertices.size();

			HeapTask(std::bind(&VertexBuffer::CreateVertexBuffer, this));
		}

		std::vector<T> GetVertices() const
		{
			return _vertices;
		}

	private:
		void CreateVertexBuffer()
		{
			ID3D12GraphicsCommandList* commandList = static_cast<ID3D12GraphicsCommandList*>(Factory::GetCommandList());

			size_t vertexBufferSize = size_t(sizeof(T) * _vertexCount);
			PrepareHeapResource(vertexBufferSize);

			HeapManager::Upload(this, &_vertices[0], int(vertexBufferSize), int(vertexBufferSize), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			// Initialize the vertex buffer view.
			_vertexBufferView.BufferLocation = _pResource->GetGPUVirtualAddress();
			_vertexBufferView.StrideInBytes = sizeof(T);
			_vertexBufferView.SizeInBytes = UINT(vertexBufferSize);
		}

		VertexBuffer<T>()
		{
			if (std::is_same<T, VertexPosCol>())
			{
				std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
				};

				_inputLayout = inputElementDescs;
				return;
			}

			if (std::is_same<T, VertexPosColUv>())
			{
				std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
				};

				_inputLayout = inputElementDescs;
				return;
			}

			if (std::is_same<T, VertexPosUv>())
			{
				std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
				};

				_inputLayout = inputElementDescs;
				return;
			}

			Logging::LogError("Unknown vertex layout.");
		}

		std::vector<T> _vertices;

		friend class VertexBufferFactory;
	};
}

