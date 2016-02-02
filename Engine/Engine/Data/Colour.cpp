#include "..\Utils\Math.h"

namespace Engine
{
	const Colour Colour::Clear = Colour(1.0f, 1.0f, 1.0f, 0.0f);
	const Colour Colour::White = Colour(1.0f, 1.0f, 1.0f);
	const Colour Colour::Red = Colour(1.0f, 0.0f, 0.0f);
	const Colour Colour::Green = Colour(0.0f, 1.0f, 0.0f);
	const Colour Colour::Blue = Colour(0.0f, 0.0f, 1.0f);
	const Colour Colour::Yellow = Colour(1.0f, 1.0f, 0.0f);
	const Colour Colour::Pink = Colour(1.0f, 0.0f, 1.0f);
	const Colour Colour::Black = Colour(0.0f, 0.0f, 0.0f);

	Colour::Colour()
		: _red(1.0f)
		, _green(1.0f)
		, _blue(1.0f)
		, _alpha(1.0f) { }

	Colour::Colour(float r, float g, float b, float a)
	{
		SetRed(r);
		SetGreen(g);
		SetBlue(b);
		SetAlpha(a);
	}

	Colour Colour::operator*(const Colour& rhs) const
	{
		return Colour(Math::Clamp(_red * rhs._red, 0.0f, 1.0f),
		              Math::Clamp(_green * rhs._green, 0.0f, 1.0f),
		              Math::Clamp(_blue * rhs._blue, 0.0f, 1.0f),
		              Math::Clamp(_alpha * rhs._alpha, 0.0f, 1.0f));
	}

	Colour Colour::operator+(const Colour& rhs) const
	{
		return Colour(Math::Clamp(_red + rhs._red, 0.0f, 1.0f),
		              Math::Clamp(_green + rhs._green, 0.0f, 1.0f),
		              Math::Clamp(_blue + rhs._blue, 0.0f, 1.0f),
		              Math::Clamp(_alpha + rhs._alpha, 0.0f, 1.0f));
	}

	Colour Colour::operator-(const Colour& rhs) const
	{
		return Colour(Math::Clamp(_red - rhs._red, 0.0f, 1.0f),
		              Math::Clamp(_green - rhs._green, 0.0f, 1.0f),
		              Math::Clamp(_blue - rhs._blue, 0.0f, 1.0f),
		              Math::Clamp(_alpha - rhs._alpha, 0.0f, 1.0f));
	}

	Colour& Colour::operator*=(const Colour& rhs)
	{
		SetRed(_red * rhs._red);
		SetGreen(_green * rhs._green);
		SetBlue(_blue * rhs._blue);
		SetAlpha(_alpha * rhs._alpha);

		return *this;
	}

	Colour& Colour::operator+=(const Colour& rhs)
	{
		SetRed(_red + rhs._red);
		SetGreen(_green + rhs._green);
		SetBlue(_blue + rhs._blue);
		SetAlpha(_alpha + rhs._alpha);

		return *this;
	}

	Colour& Colour::operator-=(const Colour& rhs)
	{
		SetRed(_red - rhs._red);
		SetGreen(_green - rhs._green);
		SetBlue(_blue - rhs._blue);
		SetAlpha(_alpha - rhs._alpha);

		return *this;
	}

	float const& Colour::GetRed() const
	{
		return _red;
	}

	float const& Colour::GetGreen() const
	{
		return _green;
	}

	float const& Colour::GetBlue() const
	{
		return _blue;
	}

	float const& Colour::GetAlpha() const
	{
		return _alpha;
	}

	void Colour::SetRed(float const& value)
	{
		_red = Math::Clamp(value, 0.0f, 1.0f);
	}

	void Colour::SetGreen(float const& value)
	{
		_green = Math::Clamp(value, 0.0f, 1.0f);
	}

	void Colour::SetBlue(float const& value)
	{
		_blue = Math::Clamp(value, 0.0f, 1.0f);
	}

	Colour::operator DirectX::XMFLOAT3() const
	{
		return DirectX::XMFLOAT3(_red, _green, _blue);
	}

	Colour::operator DirectX::XMFLOAT4() const
	{
		return DirectX::XMFLOAT4(_red, _green, _blue, _alpha);
	}

	void Colour::SetAlpha(float const& value)
	{
		_alpha = Math::Clamp(value, 0.0f, 1.0f);
	}
}

