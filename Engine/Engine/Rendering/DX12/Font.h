#pragma once

#include <unordered_map>
#include "HeapResource.h"

namespace Engine
{
	class Texture;

	class Font : HeapResource
	{
	public:
		Font();
		~Font();

		struct Letter
		{
			float LeftUv;
			float RightUv;
			int Width;
		};

		const std::string& GetName() const;

	private:
		std::string _name;
		std::unordered_map<char, Letter> _chars;
		Texture* _pTexture;

		friend class Text;
		friend class FontManager;
	};
}

