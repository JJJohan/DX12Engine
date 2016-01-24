using namespace DirectX;

namespace Engine
{
	const Vector4& Vector4::One = Vector4(1, 1, 1, 1);
	const Vector4& Vector4::Zero = Vector4(0, 0, 0, 0);

	Vector4::Vector4()
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
		, Z(_dxVector.m128_f32[2])
		, W(_dxVector.m128_f32[3])
	{
		_dxVector = XMVectorZero();
	}

	Vector4::Vector4(float x, float y, float z, float w)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
		, Z(_dxVector.m128_f32[2])
		, W(_dxVector.m128_f32[3])
	{
		_dxVector = XMVectorSet(x, y, z, w);
	}

	Vector4::Vector4(const XMVECTOR& vector)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
		, Z(_dxVector.m128_f32[2])
		, W(_dxVector.m128_f32[3])
	{
		_dxVector = vector;
	}

	Vector4::Vector4(const XMFLOAT4& vector)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
		, Z(_dxVector.m128_f32[2])
		, W(_dxVector.m128_f32[3])
	{
		_dxVector = XMLoadFloat4(&vector);
	}

	Vector4::Vector4(int x, int y, int z, int w)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
		, Z(_dxVector.m128_f32[2])
		, W(_dxVector.m128_f32[3])
	{
		_dxVector = XMVectorSet(float(x), float(y), float(z), float(w));
	}

	Vector4::Vector4(const Vector4& rhs)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
		, Z(_dxVector.m128_f32[2])
		, W(_dxVector.m128_f32[3])
	{
		_dxVector = XMVectorSet(rhs.X, rhs.Y, rhs.Z, rhs.W);
	}

	Vector4::Vector4(const Vector3& rhs)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
		, Z(_dxVector.m128_f32[2])
		, W(_dxVector.m128_f32[3])
	{
		_dxVector = XMVectorSet(rhs.X, rhs.Y, rhs.Z, 0.0f);
	}

	Vector4 Vector4::operator+(const Vector4& rhs) const
	{
		XMVECTOR v = XMVectorAdd(_dxVector, rhs._dxVector);
		return Vector4(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
	}

	Vector4 Vector4::operator*(const Vector4& rhs) const
	{
		XMVECTOR v = XMVectorMultiply(_dxVector, rhs._dxVector);
		return Vector4(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
	}

	Vector4 Vector4::operator-(const Vector4& rhs) const
	{
		XMVECTOR v = XMVectorSubtract(_dxVector, rhs._dxVector);
		return Vector4(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
	}

	Vector4 Vector4::operator/(const Vector4& rhs) const
	{
		XMVECTOR v = XMVectorDivide(_dxVector, rhs._dxVector);
		return Vector4(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
	}

	void Vector4::operator=(const Vector4& rhs)
	{
		XMVECTOR v = XMVectorSet(rhs.X, rhs.Y, rhs.Z, rhs.W);
		_dxVector = v;
	}

	Vector4 Vector4::operator*(const float& rhs) const
	{
		XMVECTOR v = XMVectorScale(_dxVector, rhs);
		return Vector4(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
	}

	Vector4 Vector4::operator/(const float& rhs) const
	{
		XMVECTOR v = XMVectorScale(_dxVector, 1.0f / rhs);
		return Vector4(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
	}

	Vector4& Vector4::operator+=(const Vector4& rhs)
	{
		_dxVector = XMVectorAdd(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector4& Vector4::operator*=(const Vector4& rhs)
	{
		_dxVector = XMVectorMultiply(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector4& Vector4::operator/=(const Vector4& rhs)
	{
		_dxVector = XMVectorDivide(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector4& Vector4::operator*=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, rhs);

		return *this;
	}

	Vector4::operator DirectX::XMFLOAT4() const
	{
		XMFLOAT4 vec;
		XMStoreFloat4(&vec, _dxVector);
		return vec;
	}

	Vector4::operator DirectX::XMVECTOR() const
	{
		return _dxVector;
	}

	Vector4& Vector4::operator/=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, 1.0f / rhs);

		return *this;
	}

	Vector4& Vector4::operator-=(const Vector4& rhs)
	{
		_dxVector = XMVectorSubtract(_dxVector, rhs._dxVector);

		return *this;
	}
}