#pragma once

#include "../stdafx.h"

namespace Engine
{
	class ResourceFactory;
}

namespace EngineNET
{
	ref class ConstantBufferInstance;
	ref class VertexBufferInstance;
	ref class IndexBufferInstance;
	ref class Material;
	ref class Texture;
	ref struct VertexType;

	public ref class ResourceFactory
	{
	public:
		static ConstantBufferInstance^ CreateConstantBuffer();
		static VertexBufferInstance^ CreateVertexBufferInstance(VertexType vertexType);
		static IndexBufferInstance^ CreateIndexBufferInstance();
		static Material^ CreateMaterial();
		static Texture^ CreateTexture();

	private:
		Engine::ResourceFactory* _pFactory;
	};
}