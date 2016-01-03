#include "Vector3.h"
#include "Vector2.h"

using namespace DirectX;

namespace Engine
{
	const Vector3 Vector3::One = Vector3(1, 1, 1);
	const Vector3 Vector3::Zero = Vector3(0, 0, 0);

	Vector3::Vector3()
		: X(0.0f)
		  , Y(0.0f)
		  , Z(0.0f)
	{
		_dxVector = XMVectorZero();
	}

	Vector3::Vector3(float x, float y, float z)
		: X(x)
		  , Y(y)
		  , Z(z)
	{
		_dxVector = XMVectorSet(x, y, z, 0.0f);
	}

	Vector3::Vector3(int x, int y, int z)
		: X(float(x))
		  , Y(float(y))
		  , Z(float(z))
	{
		_dxVector = XMVectorSetInt(x, y, z, 0);
	}

	Vector3::Vector3(const Vector2& rhs)
		: X(rhs.X)
		  , Y(rhs.Y)
		  , Z(0.0f)
	{
		_dxVector = XMVectorSet(rhs.X, rhs.Y, 0.0f, 0.0f);
	}

	Vector3 Vector3::operator+(const Vector3& rhs)
	{
		XMVECTOR v = XMVectorAdd(_dxVector, rhs._dxVector);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator*(const Vector3& rhs)
	{
		XMVECTOR v = XMVectorMultiply(_dxVector, rhs._dxVector);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator-(const Vector3& rhs)
	{
		XMVECTOR v = XMVectorSubtract(_dxVector, rhs._dxVector);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator/(const Vector3& rhs)
	{
		XMVECTOR v = XMVectorDivide(_dxVector, rhs._dxVector);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator*(const float& rhs)
	{
		XMVECTOR v = XMVectorScale(_dxVector, rhs);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator/(const float& rhs)
	{
		XMVECTOR v = XMVectorScale(_dxVector, 1.0f / rhs);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		_dxVector = XMVectorAdd(_dxVector, rhs._dxVector);
		Sync();

		return *this;
	}

	Vector3& Vector3::operator*=(const Vector3& rhs)
	{
		_dxVector = XMVectorMultiply(_dxVector, rhs._dxVector);
		Sync();

		return *this;
	}

	Vector3& Vector3::operator/=(const Vector3& rhs)
	{
		_dxVector = XMVectorDivide(_dxVector, rhs._dxVector);
		Sync();

		return *this;
	}

	Vector3& Vector3::operator*=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, rhs);
		Sync();

		return *this;
	}

	Vector3& Vector3::operator/=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, 1.0f / rhs);
		Sync();

		return *this;
	}

	DirectX::XMVECTOR Vector3::GetXMVector()
	{
		return _dxVector;
	}

	Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		_dxVector = XMVectorSubtract(_dxVector, rhs._dxVector);
		Sync();

		return *this;
	}

	inline void Vector3::Sync()
	{
		X = _dxVector.m128_f32[0];
		Y = _dxVector.m128_f32[1];
		Z = _dxVector.m128_f32[2];
	}
}

