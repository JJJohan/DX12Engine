#pragma once
#include <string>
#include "RenderObject.h"

namespace Engine
{
	class Font : RenderObject
	{
	public:
		Font();
		~Font();

		void LoadFont(std::string imageFile, std::string fontFile);
		void SetText(std::string text);

	private:

		std::string _text;
	};
}

