#include "Font.h"
#include "../../Utils/Logging.h"

namespace Engine
{
	std::unordered_map<std::string, Font> Font::_fonts = std::unordered_map<std::string, Font>();

	void Font::LoadFont(std::string fontName, std::string textureFile, std::string fontFile)
	{
		// Check if the font already exists.
		if (_fonts.find("") != _fonts.end())
		{
			Logging::LogError("The font '{0}' has already been loaded.");
			return;
		}
	}
}

