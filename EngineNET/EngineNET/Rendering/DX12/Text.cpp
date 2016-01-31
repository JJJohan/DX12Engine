#include "Text.h"
#include "Font.h"
#include "../../Data/String.h"
#include "../../Data/Colour.h"
#include "Engine/Rendering/DX12/Text.h"
#include "Engine/Rendering/DX12/Font.h"

namespace EngineNET
{
	Text::Text(Engine::Text* text)
	{
		_pText = text;
		Transform = gcnew EngineNET::Transform(&_pText->Transform);
	}

	Text::Text(System::String^ fontName)
	{
		_pText = new Engine::Text(String(fontName));
		Transform = gcnew EngineNET::Transform(&_pText->Transform);
	}

	Text::Text(System::String^ name, System::String^ fontName)
	{
		_pText = new Engine::Text(String(name), String(fontName));
		Transform = gcnew EngineNET::Transform(&_pText->Transform);
	}

	Text::Text(Font^ font)
	{
		_pText = new Engine::Text(font);
		Transform = gcnew EngineNET::Transform(&_pText->Transform);
	}

	Text::Text(System::String^ name, Font^ font)
	{
		_pText = new Engine::Text(String(name), font);
		Transform = gcnew EngineNET::Transform(&_pText->Transform);
	}

	Text::~Text()
	{
		this->!Text();
	}

	Text::!Text()
	{
		delete _pText;
	}

	Text::operator Engine::Text*()
	{
		return _pText;
	}

	void Text::Draw()
	{
		_pText->Draw();
	}

	void Text::SetText(System::String^ text)
	{
		_pText->SetText(String::Sys2Std(text));
	}

	System::String^ Text::GetText()
	{
		return String::Std2Sys(_pText->GetText());
	}

#pragma managed(push, off)
	void _SetColour(Engine::Text* text, float* vals)
	{
		text->SetColour(Engine::Colour(vals[0], vals[1], vals[2], vals[3]));
	}
#pragma managed(pop)

	void Text::SetColour(Colour^ colour)
	{
		float vals[4] = { colour->R, colour->G, colour->B, colour->A };
		_SetColour(_pText, vals);
	}

	Colour^ Text::GetColour()
	{
		return gcnew Colour(const_cast<Engine::Colour&>(_pText->GetColour()));
	}

	void Text::EnableWorldSpace(bool flag)
	{
		_pText->EnableWorldSpace(flag);
	}
}