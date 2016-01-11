#pragma once

#include <DirectXMath.h>
#include "VertexBuffer.h"
#include "../../Data/Transform.h"
#include <unordered_set>

namespace Engine
{
	class Material;
	class IndexBuffer;
	class ConstantBuffer;

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

		void Update() const;
		void Draw() const;

		Transform Transform;

	private:
		static std::unordered_set<RenderObject*> _renderObjects;

		VertexBufferBase* _pVertexBuffer;
		IndexBuffer* _pIndexBuffer;
		Material* _pMaterial;
		ConstantBuffer* _pCbuffer;

		friend class DX12Renderer;
	};
}

