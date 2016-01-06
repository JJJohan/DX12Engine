#pragma once

#include "Factory.h"

namespace Engine
{
	class Texture;

	class TextureFactory : public Factory
	{
	public:
		static Texture* CreateTexture();
	};
}

