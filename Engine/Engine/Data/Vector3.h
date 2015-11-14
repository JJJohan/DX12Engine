#pragma once

#include "DirectXMath.h"

namespace Engine
{
	class Vector2;

	class Vector3
	{
	public:
		Vector3();
		Vector3(const Vector2& rhs);
		Vector3(float x, float y, float z);
		Vector3(int x, int y, int z);

		Vector3 operator+(const Vector3& rhs);
		Vector3 operator*(const Vector3& rhs);
		Vector3 operator-(const Vector3& rhs);
		Vector3 operator/(const Vector3& rhs);
		Vector3 operator*(const float& rhs);
		Vector3 operator/(const float& rhs);

		Vector3& operator+=(const Vector3& rhs);
		Vector3& operator*=(const Vector3& rhs);
		Vector3& operator-=(const Vector3& rhs);
		Vector3& operator/=(const Vector3& rhs);
		Vector3& operator*=(const float& rhs);
		Vector3& operator/=(const float& rhs);

		DirectX::XMVECTOR GetXMVector();

		static const Vector3 Zero;
		static const Vector3 One;

		float X;
		float Y;
		float Z;

	private:
		DirectX::XMVECTOR _dxVector;
		inline void Sync();
	};
}