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
		ENGINE_API RenderObject();
		ENGINE_API virtual ~RenderObject();

		ENGINE_API void RenderObject::SetVertexBuffer(VertexBufferInstance* vertexBuffer);
		ENGINE_API VertexBufferInstance* RenderObject::GetVertexBuffer() const;

		ENGINE_API void SetIndexBuffer(IndexBufferInstance* indexBuffer);
		ENGINE_API IndexBufferInstance* GetIndexBuffer() const;

		ENGINE_API void SetMaterial(Material* material);
		ENGINE_API Material* GetMaterial() const;

		virtual void Update();
		ENGINE_API virtual void Draw();

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

