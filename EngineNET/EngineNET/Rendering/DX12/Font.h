#pragma once

#include "../../stdafx.h"

namespace Engine
{
	class Font;
}

namespace EngineNET
{
	public ref class Font
	{
	public:
		Font(Engine::Font* font);
		~Font();
		!Font();

		operator Engine::Font*();
		System::String^ GetName();

	private:
		Engine::Font* _pFont;
	};
}