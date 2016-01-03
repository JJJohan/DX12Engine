#include "Vector2.h"
#include "DirectXPackedVector.h"

using namespace DirectX;

namespace Engine
{
	const Vector2 Vector2::One = Vector2(1, 1);
	const Vector2 Vector2::Zero = Vector2(0, 0);

	Vector2::Vector2()
		: X(0.0f)
		  , Y(0.0f)
	{
		_dxVector = XMVectorZero();
	}

	Vector2::Vector2(int x, int y)
		: X(float(x))
		  , Y(float(y))
	{
		_dxVector = XMVectorSetInt(x, y, 0, 0);
	}

	Vector2::Vector2(float x, float y)
		: X(x)
		  , Y(y)
	{
		_dxVector = XMVectorSet(x, y, 0.0f, 0.0f);
	}

	Vector2 Vector2::operator+(const Vector2& rhs)
	{
		XMVECTOR v = XMVectorAdd(_dxVector, rhs._dxVector);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator*(const Vector2& rhs)
	{
		XMVECTOR v = XMVectorMultiply(_dxVector, rhs._dxVector);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator-(const Vector2& rhs)
	{
		XMVECTOR v = XMVectorSubtract(_dxVector, rhs._dxVector);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator/(const Vector2& rhs)
	{
		XMVECTOR v = XMVectorDivide(_dxVector, rhs._dxVector);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator*(const float& rhs)
	{
		XMVECTOR v = XMVectorScale(_dxVector, rhs);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator/(const float& rhs)
	{
		XMVECTOR v = XMVectorScale(_dxVector, 1.0f / rhs);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2& Vector2::operator+=(const Vector2& rhs)
	{
		_dxVector = XMVectorAdd(_dxVector, rhs._dxVector);
		Sync();

		return *this;
	}

	Vector2& Vector2::operator*=(const Vector2& rhs)
	{
		_dxVector = XMVectorMultiply(_dxVector, rhs._dxVector);
		Sync();

		return *this;
	}

	Vector2& Vector2::operator/=(const Vector2& rhs)
	{
		_dxVector = XMVectorDivide(_dxVector, rhs._dxVector);
		Sync();

		return *this;
	}

	Vector2& Vector2::operator*=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, rhs);
		Sync();

		return *this;
	}

	Vector2& Vector2::operator/=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, 1.0f / rhs);
		Sync();

		return *this;
	}

	DirectX::XMVECTOR Vector2::GetXMVector()
	{
		return _dxVector;
	}

	Vector2& Vector2::operator-=(const Vector2& rhs)
	{
		_dxVector = XMVectorSubtract(_dxVector, rhs._dxVector);
		Sync();

		return *this;
	}

	inline void Vector2::Sync()
	{
		X = _dxVector.m128_f32[0];
		Y = _dxVector.m128_f32[1];
	}
}

