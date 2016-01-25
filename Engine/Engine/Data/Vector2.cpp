using namespace DirectX;

namespace Engine
{
	const Vector2& Vector2::One = Vector2(1, 1);
	const Vector2& Vector2::Zero = Vector2(0, 0);
	const Vector2& Vector2::Up = Vector2(0, 1);
	const Vector2& Vector2::Down = Vector2(0, -1);
	const Vector2& Vector2::Left = Vector2(-1, 0);
	const Vector2& Vector2::Right = Vector2(1, 0);

	Vector2::Vector2()
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
	{
		_dxVector = XMVectorZero();
	}

	Vector2::Vector2(float x, float y)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
	{
		_dxVector = XMVectorSet(x, y, 0.0f, 0.0f);
	}

	Vector2::Vector2(const XMVECTOR& vector)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
	{
		_dxVector = vector;
	}

	Vector2::Vector2(const XMFLOAT2& vector)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
	{
		_dxVector = XMLoadFloat2(&vector);
	}

	Vector2::Vector2(int x, int y)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
	{
		_dxVector = XMVectorSet(float(x), float(y), 0.0f, 0.0f);
	}

	Vector2::Vector2(const Vector2& rhs)
		: X(_dxVector.m128_f32[0])
		, Y(_dxVector.m128_f32[1])
	{
		_dxVector = XMVectorSet(rhs.X, rhs.Y, 0.0f, 0.0f);
	}

	Vector2 Vector2::operator+(const Vector2& rhs) const
	{
		XMVECTOR v = XMVectorAdd(_dxVector, rhs._dxVector);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator*(const Vector2& rhs) const
	{
		XMVECTOR v = XMVectorMultiply(_dxVector, rhs._dxVector);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator-(const Vector2& rhs) const
	{
		XMVECTOR v = XMVectorSubtract(_dxVector, rhs._dxVector);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator/(const Vector2& rhs) const
	{
		XMVECTOR v = XMVectorDivide(_dxVector, rhs._dxVector);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	void Vector2::operator=(const Vector2& rhs)
	{
		_dxVector = XMVectorSet(rhs.X, rhs.Y, 0.0f, 0.0f);
	}

	Vector2 Vector2::operator*(const float& rhs) const
	{
		XMVECTOR v = XMVectorScale(_dxVector, rhs);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2 Vector2::operator/(const float& rhs) const
	{
		XMVECTOR v = XMVectorScale(_dxVector, 1.0f / rhs);
		return Vector2(v.m128_f32[0], v.m128_f32[1]);
	}

	Vector2& Vector2::operator+=(const Vector2& rhs)
	{
		_dxVector = XMVectorAdd(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector2& Vector2::operator*=(const Vector2& rhs)
	{
		_dxVector = XMVectorMultiply(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector2& Vector2::operator/=(const Vector2& rhs)
	{
		_dxVector = XMVectorDivide(_dxVector, rhs._dxVector);

		return *this;
	}

	Vector2& Vector2::operator*=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, rhs);

		return *this;
	}

	Vector2::operator DirectX::XMFLOAT2() const
	{
		XMFLOAT2 vec;
		XMStoreFloat2(&vec, _dxVector);
		return vec;
	}

	Vector2::operator DirectX::XMVECTOR() const
	{
		return _dxVector;
	}

	Vector2& Vector2::operator/=(const float& rhs)
	{
		_dxVector = XMVectorScale(_dxVector, 1.0f / rhs);

		return *this;
	}

	Vector2& Vector2::operator-=(const Vector2& rhs)
	{
		_dxVector = XMVectorSubtract(_dxVector, rhs._dxVector);

		return *this;
	}
}