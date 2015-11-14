#pragma once

namespace Engine
{
	class Colour
	{
	public:
		Colour();
		Colour(float r, float g, float b, float a = 1.0f);

		Colour operator*(const Colour& rhs) const;
		Colour operator+(const Colour& rhs) const;
		Colour operator-(const Colour& rhs) const;

		Colour& operator*=(const Colour& rhs);
		Colour& operator+=(const Colour& rhs);
		Colour& operator-=(const Colour& rhs);

		float const& GetRed() const;
		float const& GetGreen() const;
		float const& GetBlue() const;
		float const& GetAlpha() const;
		void SetRed(float const& value);
		void SetGreen(float const& value);
		void SetBlue(float const& value);
		void SetAlpha(float const& value);

		const static Colour Clear;
		const static Colour White;
		const static Colour Red;
		const static Colour Green;
		const static Colour Blue;
		const static Colour Yellow;
		const static Colour Pink;
		const static Colour Black;

	private:
		float _red;
		float _green;
		float _blue;
		float _alpha;
	};
}