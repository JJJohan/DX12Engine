#pragma once

#include "DirectXMath.h"

namespace Engine
{
	class Vector2
	{
	public:
		Vector2();
		Vector2(float x, float y);
		Vector2(int x, int y);

		Vector2 operator+(const Vector2& rhs);
		Vector2 operator*(const Vector2& rhs);
		Vector2 operator-(const Vector2& rhs);
		Vector2 operator/(const Vector2& rhs);
		Vector2 operator*(const float& rhs);
		Vector2 operator/(const float& rhs);

		Vector2& operator+=(const Vector2& rhs);
		Vector2& operator*=(const Vector2& rhs);
		Vector2& operator-=(const Vector2& rhs);
		Vector2& operator/=(const Vector2& rhs);
		Vector2& operator*=(const float& rhs);
		Vector2& operator/=(const float& rhs);

		DirectX::XMVECTOR GetXMVector();

		static const Vector2 Zero;
		static const Vector2 One;

		float X;
		float Y;

	private:
		DirectX::XMVECTOR _dxVector;
		inline void Sync();
	};
}