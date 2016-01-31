#pragma once

namespace Engine
{
	class ResourceFactory;
	enum VertexType;
}

namespace EngineNET
{
	ref class VertexBufferInstance;
	ref class IndexBufferInstance;
	ref class Material;
	ref class Texture;

	public ref class ResourceFactory
	{
	public:
		static VertexBufferInstance^ CreateVertexBufferInstance();
		static IndexBufferInstance^ CreateIndexBufferInstance();
		static Material^ CreateMaterial();
		static Texture^ CreateTexture();

	private:
		Engine::ResourceFactory* _pFactory;
	};
}