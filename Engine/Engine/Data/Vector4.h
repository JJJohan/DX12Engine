#pragma once

#include "DirectXMath.h"

namespace Engine
{
	class Vector3;

	class Vector4
	{
	public:
		ENGINE_API Vector4();
		ENGINE_API Vector4(const Vector3& rhs);
		ENGINE_API Vector4(const Vector4& rhs);
		ENGINE_API Vector4(float x, float y, float z, float w);
		ENGINE_API Vector4(int x, int y, int z, int w);
		ENGINE_API Vector4(const DirectX::XMVECTOR& vector);
		ENGINE_API Vector4(const DirectX::XMFLOAT4& vector);

		ENGINE_API Vector4 operator+(const Vector4& rhs) const;
		ENGINE_API Vector4 operator*(const Vector4& rhs) const;
		ENGINE_API Vector4 operator-(const Vector4& rhs) const;
		ENGINE_API Vector4 operator/(const Vector4& rhs) const;
		ENGINE_API Vector4 operator*(const float& rhs) const;
		ENGINE_API Vector4 operator/(const float& rhs) const;
		ENGINE_API void operator=(const Vector4& rhs);

		ENGINE_API Vector4& operator+=(const Vector4& rhs);
		ENGINE_API Vector4& operator*=(const Vector4& rhs);
		ENGINE_API Vector4& operator-=(const Vector4& rhs);
		ENGINE_API Vector4& operator/=(const Vector4& rhs);
		ENGINE_API Vector4& operator*=(const float& rhs);
		ENGINE_API Vector4& operator/=(const float& rhs);

		ENGINE_API operator DirectX::XMFLOAT4() const;
		ENGINE_API operator DirectX::XMVECTOR() const;

		static ENGINE_API const Vector4& Zero;
		static ENGINE_API const Vector4& One;

		float& X;
		float& Y;
		float& Z;
		float& W;

	private:
		DirectX::XMVECTOR _dxVector;
	};
}

