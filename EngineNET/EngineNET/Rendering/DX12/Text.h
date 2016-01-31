#pragma once

#include "../../stdafx.h"
#include "RenderObject.h"

namespace Engine
{
	class Text;
}

namespace EngineNET
{
	ref class Font;
	ref class Colour;

	public ref class Text : public RenderObject
	{
	public:
		Text(Engine::Text* text);
		Text(System::String^ fontName);
		Text(System::String^ name, System::String^ fontName);
		Text(Font^ font);
		Text(System::String^ name, Font^ font);
		~Text();
		!Text();
		
		operator Engine::Text*();

		void Draw() override;

		void SetText(System::String^ text);
		System::String^ GetText();
		void SetColour(Colour^ colour);
		Colour^ GetColour();
		void EnableWorldSpace(bool flag);

	private:
		Engine::Text* _pText;
	};
}