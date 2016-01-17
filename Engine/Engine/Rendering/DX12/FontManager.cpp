#include <fstream>
#include "FontManager.h"
#include "Texture.h"
#include "../../Data/String.h"
#include "../../Utils/Logging.h"
#include "../../Factory/ResourceFactory.h"
#include "Font.h"

namespace Engine
{
	std::unordered_map<std::string, Font*> FontManager::_fonts;

	Font* FontManager::LoadFont(std::string fontName, std::string textureFile, std::string fontFile)
	{
		// Check if the font already exists.
		if (_fonts.find(fontName) != _fonts.end())
		{
			Logging::LogError("The font '{0}' has already been loaded.", fontName);
			return nullptr;
		}

		// Load the font texture.
		Texture* texture = ResourceFactory::CreateTexture();
		if (!texture->Load(textureFile))
		{
			// Textures have their own error handling.
			return nullptr;
		}

		// Create the font instance.
		Font* font = new Font();
		font->_name = fontName;
		font->_pTexture = texture;

		// Load the font layout.
		if (!LoadLayout(fontFile, font))
		{
			Logging::LogError("Failed to load layout for font '{0}'.", fontName);
		}

		// Store the font.
		_fonts[fontName] = font;

		return font;
	}

	const Font* FontManager::GetFont(std::string fontName)
	{
		if (_fonts.find(fontName) != _fonts.end())
		{
			return _fonts[fontName];
		}

		Logging::LogError("Font '{0}' does not exist.", fontName);
		return nullptr;
	}

	bool FontManager::LoadLayout(std::string fontFile, Font* font)
	{
		// Open the file.
		std::ifstream layout = std::ifstream(fontFile, std::ios_base::ate);
		if (!layout.is_open())
		{
			return false;
		}

		// Read file into buffer.
		std::streamoff length = layout.tellg();
		char* buffer = new char[size_t(length)];
		layout.seekg(0, layout.beg);
		layout.read(buffer, length);
		layout.close();

		// Split lines.
		String text = buffer;
		std::vector<String> lines = text.Split('\n');
		delete buffer;

		// Populate character map.
		bool heightInfo = false;
		size_t size = lines.size();
		for (int i = 0; i < size; ++i)
		{
			// Ignore comments.
			if (lines[i][0] == '/' && lines[i][1] == '/')
			{
				continue;
			}

			// Split by spaces.
			std::vector<String> bits = lines[i].Split('\t');

			// Get height info.
			if (!heightInfo)
			{
				if (bits.size() == 4)
				{
					font->_topUv = bits[0].ToFloat();
					font->_bottomUv = bits[1].ToFloat();
					font->_widthScale = bits[2].ToFloat();
					font->_heightScale = bits[3].ToFloat();
					heightInfo = true;
					continue;
				}
			}

			// Get character info.
			if (bits.size() != 4)
			{
				continue;
			}

			// Parse data.
			char letter = bits[0][0];
			float left = bits[1].ToFloat();
			float right = bits[2].ToFloat();
			int width = bits[3].ToInt();

			font->_chars[letter] = Font::Letter{left, right, width};
		}

		// Add space.
		font->_chars[' '] = Font::Letter{0.99f, 0.999f, 4};

		return true;
	}
}

