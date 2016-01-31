#pragma once

#include "../../stdafx.h"

namespace Engine
{
	class Texture;
}

namespace EngineNET
{
	public ref class Texture
	{
	public:
		Texture(Engine::Texture* texture);
		~Texture();
		!Texture();

		operator Engine::Texture*();

		bool Load(System::String^ filePath);

	private:
		Engine::Texture* _pTexture;
	};
}