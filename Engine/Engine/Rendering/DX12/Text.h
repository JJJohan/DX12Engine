#pragma once

#include <mutex>
#include "RenderObject.h"
#include "../../Data/Colour.h"

namespace Engine
{
	class Font;

	class Text : public RenderObject
	{
	public:
		ENGINE_API Text(std::string fontName);
		ENGINE_API Text(Font* font);
		ENGINE_API ~Text();

		ENGINE_API void Draw() override;
		void Update() override;

		ENGINE_API void SetText(std::string text);
		ENGINE_API std::string GetText() const;
		ENGINE_API void SetColour(Colour colour);
		ENGINE_API const Colour& GetColour() const;
		ENGINE_API void EnableWorldSpace(bool flag);

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
		std::mutex _textChangeMutex;
	};
}

