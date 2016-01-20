#pragma once

#include "DirectXMath.h"

namespace Engine
{
	class Vector2
	{
	public:
		ENGINE_API Vector2();
		ENGINE_API Vector2(float x, float y);
		ENGINE_API Vector2(int x, int y);

		ENGINE_API Vector2 operator+(const Vector2& rhs) const;
		ENGINE_API Vector2 operator*(const Vector2& rhs) const;
		ENGINE_API Vector2 operator-(const Vector2& rhs) const;
		ENGINE_API Vector2 operator/(const Vector2& rhs) const;
		ENGINE_API Vector2 operator*(const float& rhs) const;
		ENGINE_API Vector2 operator/(const float& rhs) const;

		ENGINE_API Vector2& operator+=(const Vector2& rhs);
		ENGINE_API Vector2& operator*=(const Vector2& rhs);
		ENGINE_API Vector2& operator-=(const Vector2& rhs);
		ENGINE_API Vector2& operator/=(const Vector2& rhs);
		ENGINE_API Vector2& operator*=(const float& rhs);
		ENGINE_API Vector2& operator/=(const float& rhs);

		ENGINE_API DirectX::XMVECTOR GetXMVector() const;

		static ENGINE_API const Vector2 Zero;
		static ENGINE_API const Vector2 One;

		float X;
		float Y;

	private:
		DirectX::XMVECTOR _dxVector;
		inline void Sync();
	};
}

