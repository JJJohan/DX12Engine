using namespace DirectX;

namespace Engine
{
	const Vector3& Vector3::One = Vector3(1, 1, 1);
	const Vector3& Vector3::Zero = Vector3(0, 0, 0);
	const Vector3& Vector3::Up = Vector3(0, 1, 0);
	const Vector3& Vector3::Forward = Vector3(0, 0, 1);
	const Vector3& Vector3::Right = Vector3(1, 0, 0);

	Vector3::Vector3()
		: X(_dxVector.m128_f32[0])
		  , Y(_dxVector.m128_f32[1])
		  , Z(_dxVector.m128_f32[2])
	{
		_dxVector = XMVectorZero();
	}

	Vector3::Vector3(float x, float y, float z)
		: X(_dxVector.m128_f32[0])
		  , Y(_dxVector.m128_f32[1])
		  , Z(_dxVector.m128_f32[2])
	{
		_dxVector = XMVectorSet(x, y, z, 0.0f);
	}

	Vector3::Vector3(const XMVECTOR& vector)
		: X(_dxVector.m128_f32[0])
		  , Y(_dxVector.m128_f32[1])
		  , Z(_dxVector.m128_f32[2])
	{
		_dxVector = vector;
	}

	Vector3::Vector3(const XMFLOAT3& vector)
		: X(_dxVector.m128_f32[0])
		  , Y(_dxVector.m128_f32[1])
		  , Z(_dxVector.m128_f32[2])
	{
		_dxVector = XMLoadFloat3(&vector);
	}

	Vector3::Vector3(int x, int y, int z)
		: X(_dxVector.m128_f32[0])
		  , Y(_dxVector.m128_f32[1])
		  , Z(_dxVector.m128_f32[2])
	{
		_dxVector = XMVectorSet(float(x), float(y), float(z), 0.0f);
	}

	Vector3::Vector3(const Vector3& rhs)
		: X(_dxVector.m128_f32[0])
		  , Y(_dxVector.m128_f32[1])
		  , Z(_dxVector.m128_f32[2])
	{
		_dxVector = XMVectorSet(rhs.X, rhs.Y, rhs.Z, 0.0f);
	}

	Vector3::Vector3(const Vector2& rhs)
		: X(_dxVector.m128_f32[0])
		  , Y(_dxVector.m128_f32[1])
		  , Z(_dxVector.m128_f32[2])
	{
		_dxVector = XMVectorSet(rhs.X, rhs.Y, 0.0f, 0.0f);
	}

	Vector3 Vector3::operator+(const Vector3& rhs) const
	{
		XMVECTOR v = XMVectorAdd(_dxVector, rhs._dxVector);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator*(const Vector3& rhs) const
	{
		XMVECTOR v = XMVectorMultiply(_dxVector, rhs._dxVector);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator-(const Vector3& rhs) const
	{
		XMVECTOR v = XMVectorSubtract(_dxVector, rhs._dxVector);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator/(const Vector3& rhs) const
	{
		XMVECTOR v = XMVectorDivide(_dxVector, rhs._dxVector);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	void Vector3::operator=(const Vector3& rhs)
	{
		_dxVector = XMVectorSet(rhs.X, rhs.Y, rhs.Z, 0.0f);
	}

	Vector3 Vector3::operator*(const float& rhs) const
	{
		XMVECTOR v = XMVectorScale(_dxVector, rhs);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3 Vector3::operator/(const float& rhs) const
	{
		XMVECTOR v = XMVectorScale(_dxVector, 1.0f / rhs);
		return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
	}

	Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		_dxVector = XMVectorAdd(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector3& Vector3::operator*=(const Vector3& rhs)
	{
		_dxVector = XMVectorMultiply(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector3& Vector3::operator/=(const Vector3& rhs)
	{
		_dxVector = XMVectorDivide(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector3& Vector3::operator*=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, rhs);

		return *this;
	}

	Vector3::operator DirectX::XMFLOAT3() const
	{
		XMFLOAT3 vec;
		XMStoreFloat3(&vec, _dxVector);
		return vec;
	}

	Vector3::operator DirectX::XMVECTOR() const
	{
		return _dxVector;
	}

	Vector3& Vector3::operator/=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, 1.0f / rhs);

		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		_dxVector = XMVectorSubtract(_dxVector, rhs._dxVector);

		return *this;
	}
}

