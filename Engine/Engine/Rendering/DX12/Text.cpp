#include "Text.h"

namespace Engine
{
	Text::Text()
		: _pFont(nullptr)
	{
	}

	void Text::Draw()
	{
	}

	void Text::SetText(std::string text)
	{
		_text = text;
		//size_t letters = text.size();

		// Update vertex buffer
		std::vector<VertexPosColUv> vertices;

		// Updated index buffer
	}

	std::string Text::GetText() const
	{
		return _text;
	}
}

