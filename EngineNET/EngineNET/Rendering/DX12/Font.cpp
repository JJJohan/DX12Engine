#include "Font.h"
#include "../../Data/String.h"
#include "Engine/Rendering/DX12/Font.h"

namespace EngineNET
{
	Font::Font(Engine::Font* font)
	{
		_pFont = font;
	}

	Font::~Font()
	{
		this->!Font();
	}

	Font::!Font()
	{
		delete _pFont;
	}

	Font::operator Engine::Font*()
	{
		return _pFont;
	}

	System::String^ Font::GetName()
	{
		return String(_pFont->GetName());
	}
}