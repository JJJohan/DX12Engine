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

		void RenderObject::SetVertexBuffer(VertexBufferInstance* vertexBuffer);
		VertexBufferInstance* RenderObject::GetVertexBuffer() const;

		void SetIndexBuffer(IndexBufferInstance* indexBuffer);
		IndexBufferInstance* GetIndexBuffer() const;

		void SetMaterial(Material* material);
		Material* GetMaterial() const;

		virtual void Update();
		virtual void Draw();

		Transform Transform;

	protected:
		VertexBufferInstance* _pVertexBuffer;
		IndexBufferInstance* _pIndexBuffer;
		ConstantBufferInstance* _pCbuffer;
		Material* _pMaterial;

	private:
		static std::unordered_set<RenderObject*> _renderObjects;

		friend class DX12Renderer;
	};
}

