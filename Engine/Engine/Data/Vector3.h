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
		Vector3(const Vector3& rhs);
		Vector3(float x, float y, float z);
		Vector3(int x, int y, int z);
		Vector3(const DirectX::XMVECTOR& vector);
		Vector3(const DirectX::XMFLOAT3& vector);

		Vector3 operator+(const Vector3& rhs) const;
		Vector3 operator*(const Vector3& rhs) const;
		Vector3 operator-(const Vector3& rhs) const;
		Vector3 operator/(const Vector3& rhs) const;
		Vector3 operator*(const float& rhs) const;
		Vector3 operator/(const float& rhs) const;
		void operator=(const Vector3& rhs);

		Vector3& operator+=(const Vector3& rhs);
		Vector3& operator*=(const Vector3& rhs);
		Vector3& operator-=(const Vector3& rhs);
		Vector3& operator/=(const Vector3& rhs);
		Vector3& operator*=(const float& rhs);
		Vector3& operator/=(const float& rhs);

		operator DirectX::XMFLOAT3() const;
		operator DirectX::XMVECTOR() const;

		static const Vector3& Zero;
		static const Vector3& One;
		static const Vector3& Up;
		static const Vector3& Forward;
		static const Vector3& Right;

		float& X;
		float& Y;
		float& Z;

	private:
		DirectX::XMVECTOR _dxVector;
	};
}

