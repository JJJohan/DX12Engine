#pragma once

#include "../../stdafx.h"

namespace Engine
{
	class RenderObject;
}

namespace EngineNET
{
	ref class IndexBufferInstance;
	ref class VertexBufferInstance;
	ref class Material;
	ref class Transform;

	public ref class RenderObject
	{
	public:
		RenderObject();
		RenderObject(Engine::RenderObject* renderObject);
		RenderObject(System::String^ name);
		virtual ~RenderObject();
		!RenderObject();

		operator Engine::RenderObject*();

		void RenderObject::SetVertexBuffer(VertexBufferInstance^ vertexBuffer);
		VertexBufferInstance^ RenderObject::GetVertexBuffer();

		void SetIndexBuffer(IndexBufferInstance^ indexBuffer);
		IndexBufferInstance^ GetIndexBuffer();

		void SetMaterial(Material^ material);
		Material^ GetMaterial();

		virtual void Draw();

		Transform^ Transform;

	private:
		Engine::RenderObject* _pRenderObject;
	};
}