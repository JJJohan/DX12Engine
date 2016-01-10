#include <fstream>
#include "Font.h"
#include "Texture.h"

namespace Engine
{
	Font::Font()
		: _pTexture(nullptr)
	{
	}

	Font::~Font()
	{
		if (_pTexture != nullptr)
		{
			delete _pTexture;
		}
	}

	const std::string& Font::GetName() const
	{
		return _name;
	}
}

