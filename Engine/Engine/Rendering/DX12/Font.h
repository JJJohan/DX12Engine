#pragma once

#include <string>
#include <unordered_map>
#include "HeapResource.h"

namespace Engine
{
	class Font : HeapResource
	{
	public:
		static void LoadFont(std::string fontName, std::string textureFile, std::string fontFile);

	private:
		struct Letter
		{
			float LeftUv;
			float RightUv;
			float Width;
		};

		std::string _name;
		std::unordered_map<char, Letter> _chars;

		static std::unordered_map<std::string, Font> _fonts;

		friend class Text;
	};
}

