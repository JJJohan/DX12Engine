#pragma once

namespace DirectX
{
	struct XMFLOAT3;
	struct XMFLOAT4;
}

namespace Engine
{
	class Colour
	{
	public:
		ENGINE_API Colour();
		ENGINE_API Colour(float r, float g, float b, float a = 1.0f);

		ENGINE_API Colour operator*(const Colour& rhs) const;
		ENGINE_API Colour operator+(const Colour& rhs) const;
		ENGINE_API Colour operator-(const Colour& rhs) const;

		ENGINE_API Colour& operator*=(const Colour& rhs);
		ENGINE_API Colour& operator+=(const Colour& rhs);
		ENGINE_API Colour& operator-=(const Colour& rhs);

		ENGINE_API float const& GetRed() const;
		ENGINE_API float const& GetGreen() const;
		ENGINE_API float const& GetBlue() const;
		ENGINE_API float const& GetAlpha() const;
		ENGINE_API void SetRed(float const& value);
		ENGINE_API void SetGreen(float const& value);
		ENGINE_API void SetBlue(float const& value);
		ENGINE_API void SetAlpha(float const& value);

		ENGINE_API operator DirectX::XMFLOAT3() const;
		ENGINE_API operator DirectX::XMFLOAT4() const;

		const static ENGINE_API Colour Clear;
		const static ENGINE_API Colour White;
		const static ENGINE_API Colour Red;
		const static ENGINE_API Colour Green;
		const static ENGINE_API Colour Blue;
		const static ENGINE_API Colour Yellow;
		const static ENGINE_API Colour Pink;
		const static ENGINE_API Colour Black;

	private:
		float _red;
		float _green;
		float _blue;
		float _alpha;
	};
}

