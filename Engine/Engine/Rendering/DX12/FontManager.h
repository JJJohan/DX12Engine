#pragma once

#include <unordered_map>
#include "Font.h"

namespace Engine
{
	class FontManager
	{
	public:
		static Font* LoadFont(std::string fontName, std::string textureFile, std::string fontFile);
		static const Font* GetFont(std::string fontName);

	private:
		static bool LoadLayout(std::string fontFile, std::unordered_map<char, Font::Letter>& characters);
		static std::unordered_map<std::string, Font*> _fonts;
	};
}

