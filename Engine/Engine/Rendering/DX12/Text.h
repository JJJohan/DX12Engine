#pragma once
#include <string>
#include "RenderObject.h"

namespace Engine
{
	class Font;

	class Text : RenderObject
	{
	public:
		Text();
		void Draw();

		void SetText(std::string text);
		std::string GetText() const;

	private:
		std::string _text;
		Font* _pFont;
	};
}

