#pragma once
#include <string>
#include "RenderObject.h"
#include "../../Data/Colour.h"

namespace Engine
{
	class Font;

	class Text : public RenderObject
	{
	public:
		Text(std::string fontName);
		Text(Font* font);
		~Text();

		void Draw() override;
		void Update() override;

		void SetText(std::string text);
		std::string GetText() const;
		void SetColour(Colour colour);
		const Colour& GetColour() const;
		void EnableWorldSpace(bool flag);

	private:
		void ChangeText();
		void RegisterTransform();

		Vector3 SetScale(float x, float y, float z) const;
		Vector3 SetPosition(float x, float y, float z) const;

		Vector3 GetPosition(float x, float y, float z) const;
		Vector3 GetScale(float x, float y, float z) const;	

		bool _worldSpace;
		std::string _text;
		const Font* _pFont;
		Colour _colour;
		bool _textDirty;
		bool _colDirty;
	};
}

