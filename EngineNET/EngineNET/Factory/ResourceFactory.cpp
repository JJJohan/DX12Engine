#include "ResourceFactory.h"
#include "Engine/Factory/ResourceFactory.h"
#include "Engine/Rendering/DX12/VertexBufferInstance.h"
#include "../Rendering/DX12/Buffers/VertexBufferInstance.h"
#include "../Rendering/DX12/Buffers/IndexBufferInstance.h"
#include "../Rendering/DX12/Material.h"
#include "../Rendering/DX12/Texture.h"

namespace EngineNET
{
	VertexBufferInstance^ ResourceFactory::CreateVertexBufferInstance()
	{
		return gcnew VertexBufferInstance(Engine::ResourceFactory::CreateVertexBufferInstance(Engine::VertexType::VERTEX_POS_COL_UV));
	}

	IndexBufferInstance^ ResourceFactory::CreateIndexBufferInstance()
	{
		return gcnew IndexBufferInstance(Engine::ResourceFactory::CreateIndexBufferInstance());
	}

	Material^ ResourceFactory::CreateMaterial()
	{
		return gcnew Material(Engine::ResourceFactory::CreateMaterial());
	}

	Texture^ ResourceFactory::CreateTexture()
	{
		return gcnew Texture(Engine::ResourceFactory::CreateTexture());
	}
}