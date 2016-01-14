#pragma once

#include "../../Data/Transform.h"
#include <unordered_set>

namespace Engine
{
	class Material;
	class VertexBufferInstance;
	class IndexBufferInstance;
	class ConstantBuffer;

	class RenderObject
	{
	public:
		RenderObject();
		virtual ~RenderObject();

		void RenderObject::SetVertexBuffer(VertexBufferInstance* vertexBuffer)
		{
			_pVertexBuffer = vertexBuffer;
		}

		VertexBufferInstance* RenderObject::GetVertexBuffer() const
		{
			return _pVertexBuffer;
		}

		void SetIndexBuffer(IndexBufferInstance* indexBuffer);
		IndexBufferInstance* GetIndexBuffer() const;

		void SetMaterial(Material* material);
		Material* GetMaterial() const;

		virtual void Update() const;
		virtual void Draw() const;

		Transform Transform;

	private:
		static std::unordered_set<RenderObject*> _renderObjects;

		VertexBufferInstance* _pVertexBuffer;
		IndexBufferInstance* _pIndexBuffer;
		Material* _pMaterial;
		ConstantBuffer* _pCbuffer;

		friend class DX12Renderer;
	};
}

