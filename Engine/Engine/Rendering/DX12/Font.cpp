#include <fstream>
#include "Font.h"
#include "Texture.h"
#include "../../Utils/Logging.h"

namespace Engine
{
	Font::Font()
		: _pTexture(nullptr)
		, _topUv(0)
		, _bottomUv(1)
		, _widthScale(1)
		, _heightScale(1)
	{
	}

	Font::~Font()
	{
		if (_pTexture != nullptr)
		{
			delete _pTexture;
		}
	}

	Font::Letter Font::GetLetter(char letter) const
	{
		if (_chars.find(letter) == _chars.end())
		{
			Logging::LogError("Could not find letter '{0}' in font '{1}'.", letter, _name);
			return Letter();
		}

		return _chars.at(letter);
	}

	const std::string& Font::GetName() const
	{
		return _name;
	}
}

