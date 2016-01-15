#pragma once

#include <unordered_set>
#include "../../Data/Transform.h"

namespace Engine
{
	class Material;
	class VertexBufferInstance;
	class IndexBufferInstance;
	class ConstantBufferInstance;

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
		ConstantBufferInstance* _pCbuffer;
		Material* _pMaterial;

		friend class DX12Renderer;
	};
}

