#pragma once

#include "../../stdafx.h"

namespace EngineNET
{
	ref class Font;

	public ref class FontManager
	{
	public:
		static Font^ LoadFont(System::String^ fontName, System::String^ textureFile, System::String^ fontFile);
		static Font^ GetFont(System::String^ fontName);
	};
}

