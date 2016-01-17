#pragma once

#include <unordered_map>

namespace Engine
{
	class Font;

	class FontManager
	{
	public:
		static Font* LoadFont(std::string fontName, std::string textureFile, std::string fontFile);
		static const Font* GetFont(std::string fontName);

	private:
		static bool LoadLayout(std::string fontFile, Font* font);
		static std::unordered_map<std::string, Font*> _fonts;
	};
}

