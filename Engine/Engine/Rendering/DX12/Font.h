#pragma once

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

		Letter GetLetter(char letter) const;
		ENGINE_API const std::string& GetName() const;

	private:
		std::string _name;
		std::unordered_map<char, Letter> _chars;
		Texture* _pTexture;
		float _topUv;
		float _bottomUv;
		float _widthScale;
		float _heightScale;

		friend class Text;
		friend class FontManager;
	};
}

