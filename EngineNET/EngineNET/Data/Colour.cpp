#include "Colour.h"
#include "Engine/Data/Colour.h"

namespace EngineNET
{
	static Colour::Colour()
	{
		White = gcnew Colour();
		Clear = gcnew Colour(0.0f, 0.0f, 0.0f, 0.0f);
		Red = gcnew Colour(1.0f, 0.0f, 0.0f, 1.0f);
		Green = gcnew Colour(0.0f, 1.0f, 0.0f, 1.0f);
		Blue = gcnew Colour(0.0f, 0.0f, 1.0f, 1.0f);
		Yellow = gcnew Colour(1.0f, 1.0f, 0.0f, 1.0f);
		Pink = gcnew Colour(1.0f, 0.0f, 1.0f, 1.0f);
		Black = gcnew Colour(0.0f, 0.0f, 0.0f, 1.0f);
	}

	Colour::Colour()
	{
		_pColour = new Engine::Colour();
	}

	Colour::Colour(float r, float g, float b)
	{
		_pColour = new Engine::Colour(r, g, b, 1.0f);
	}

	Colour::Colour(float r, float g, float b, float a)
	{
		_pColour = new Engine::Colour(r, g, b, a);
	}

	Colour::Colour(Engine::Colour& vec)
	{
		_pColour = new Engine::Colour(vec);
	}

	Colour::~Colour()
	{
		this->!Colour();
	}

	Colour::!Colour()
	{
		delete _pColour;
	}

	Colour^ Colour::operator*(Colour^ rhs)
	{
		Engine::Colour* col = rhs->_pColour;
		Engine::Colour out = _pColour->operator*(*col);
		return gcnew Colour(out.GetRed(), out.GetGreen(), out.GetBlue(), out.GetAlpha());
	}
	Colour^ Colour::operator+(Colour^ rhs)
	{
		Engine::Colour* col = rhs->_pColour;
		Engine::Colour out = _pColour->operator+(*col);
		return gcnew Colour(out.GetRed(), out.GetGreen(), out.GetBlue(), out.GetAlpha());
	}

	Colour^ Colour::operator-(Colour^ rhs)
	{
		Engine::Colour* col = rhs->_pColour;
		Engine::Colour out = _pColour->operator-(*col);
		return gcnew Colour(out.GetRed(), out.GetGreen(), out.GetBlue(), out.GetAlpha());
	}

	Colour^ Colour::operator*=(Colour^ rhs)
	{
		Engine::Colour* col = rhs->_pColour;
		Engine::Colour out = _pColour->operator*=(*col);
		_pColour->SetRed(out.GetRed());
		_pColour->SetRed(out.GetGreen());
		_pColour->SetRed(out.GetBlue());
		_pColour->SetRed(out.GetAlpha());
		return this;
	}

	Colour^ Colour::operator+=(Colour^ rhs)
	{
		Engine::Colour* col = rhs->_pColour;
		Engine::Colour out = _pColour->operator+=(*col);
		_pColour->SetRed(out.GetRed());
		_pColour->SetRed(out.GetGreen());
		_pColour->SetRed(out.GetBlue());
		_pColour->SetRed(out.GetAlpha());
		return this;
	}

	Colour^ Colour::operator-=(Colour^ rhs)
	{
		Engine::Colour* col = rhs->_pColour;
		Engine::Colour out = _pColour->operator-=(*col);
		_pColour->SetRed(out.GetRed());
		_pColour->SetRed(out.GetGreen());
		_pColour->SetRed(out.GetBlue());
		_pColour->SetRed(out.GetAlpha());
		return this;
	}

	Colour::operator Engine::Colour*()
	{
		return _pColour;
	}

	float Colour::R::get()
	{
		return _pColour->GetRed();
	}

	void Colour::R::set(float v)
	{
		return _pColour->SetRed(v);
	}

	float Colour::G::get()
	{
		return _pColour->GetGreen();
	}

	void Colour::G::set(float v)
	{
		return _pColour->SetGreen(v);
	}

	float Colour::B::get()
	{
		return _pColour->GetBlue();
	}

	void Colour::B::set(float v)
	{
		return _pColour->SetBlue(v);
	}

	float Colour::A::get()
	{
		return _pColour->GetAlpha();
	}

	void Colour::A::set(float v)
	{
		return _pColour->SetAlpha(v);
	}
}