#pragma once

#include <DirectXMath.h>
#include "VertexBuffer.h"
#include "../../Data/Transform.h"

namespace Engine
{
	class Material;
	class IndexBuffer;

	class RenderObject
	{
	public:
		RenderObject();
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

		Transform Transform;

	private:
		VertexBufferBase* _pVertexBuffer;
		IndexBuffer* _pIndexBuffer;
		Material* _pMaterial;

		XMMATRIX _worldMatrix;

		friend class RenderObjectFactory;
	};
}

