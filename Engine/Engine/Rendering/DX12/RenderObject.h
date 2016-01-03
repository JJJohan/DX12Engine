#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "VertexBuffer.h"

using namespace Microsoft::WRL;

namespace Engine
{
	class Material;
	class IndexBuffer;

	class RenderObject
	{
	public:
		virtual ~RenderObject();

		template <typename T>
		void RenderObject::SetVertexBuffer(VertexBuffer<T>* vertexBuffer)
		{
			_pVertexBuffer = static_cast<VertexBufferBase*>(vertexBuffer);
		}

		VertexBufferBase* RenderObject::GetVertexBuffer() const
		{
			return _pVertexBuffer;
		}

		void SetIndexBuffer(IndexBuffer* indexBuffer);
		IndexBuffer* GetIndexBuffer() const;

		void SetMaterial(Material* material);
		Material* GetMaterial() const;

		void Draw() const;

	private:
		RenderObject();

		VertexBufferBase* _pVertexBuffer;
		IndexBuffer* _pIndexBuffer;
		Material* _pMaterial;

		ComPtr<ID3D12Device> _device;

		XMMATRIX _worldMatrix;

		friend class RenderObjectFactory;
	};
}

