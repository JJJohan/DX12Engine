#include "Texture.h"
#include "../../Data/String.h"
#include "Engine/Rendering/DX12/Texture.h"

namespace EngineNET
{
	Texture::Texture(Engine::Texture* texture)
	{
		_pTexture = texture;
	}

	Texture::~Texture()
	{
		this->!Texture();
	}

	Texture::!Texture()
	{
		delete _pTexture;
	}

	Texture::operator Engine::Texture*()
	{
		return _pTexture;
	}

	bool Texture::Load(System::String^ filePath)
	{
		return _pTexture->Load(String::Sys2Std(filePath));
	}
}