#pragma once

#include "DirectXMath.h"

namespace Engine
{
	class Vector2;

	class Vector3
	{
	public:
		ENGINE_API Vector3();
		ENGINE_API Vector3(const Vector2& rhs);
		ENGINE_API Vector3(const Vector3& rhs);
		ENGINE_API Vector3(float x, float y, float z);
		ENGINE_API Vector3(int x, int y, int z);
		ENGINE_API Vector3(const DirectX::XMVECTOR& vector);
		ENGINE_API Vector3(const DirectX::XMFLOAT3& vector);

		ENGINE_API Vector3 operator+(const Vector3& rhs) const;
		ENGINE_API Vector3 operator*(const Vector3& rhs) const;
		ENGINE_API Vector3 operator-(const Vector3& rhs) const;
		ENGINE_API Vector3 operator/(const Vector3& rhs) const;
		ENGINE_API Vector3 operator*(const float& rhs) const;
		ENGINE_API Vector3 operator/(const float& rhs) const;
		ENGINE_API void operator=(const Vector3& rhs);

		ENGINE_API Vector3& operator+=(const Vector3& rhs);
		ENGINE_API Vector3& operator*=(const Vector3& rhs);
		ENGINE_API Vector3& operator-=(const Vector3& rhs);
		ENGINE_API Vector3& operator/=(const Vector3& rhs);
		ENGINE_API Vector3& operator*=(const float& rhs);
		ENGINE_API Vector3& operator/=(const float& rhs);

		ENGINE_API operator DirectX::XMFLOAT3() const;
		ENGINE_API operator DirectX::XMVECTOR() const;

		static ENGINE_API const Vector3& Zero;
		static ENGINE_API const Vector3& One;
		static ENGINE_API const Vector3& Up;
		static ENGINE_API const Vector3& Forward;
		static ENGINE_API const Vector3& Right;

		float& X;
		float& Y;
		float& Z;

	private:
		DirectX::XMVECTOR _dxVector;
	};
}

