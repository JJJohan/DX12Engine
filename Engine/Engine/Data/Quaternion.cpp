#include "Quaternion.h"
#include "Vector3.h"

using namespace DirectX;

namespace Engine
{
	const Quaternion& Quaternion::Identity = Quaternion();

	Quaternion::Quaternion()
		: X(_dxQuaternion.m128_f32[0])
		  , Y(_dxQuaternion.m128_f32[1])
		  , Z(_dxQuaternion.m128_f32[2])
		  , W(_dxQuaternion.m128_f32[3])
	{
		_dxQuaternion = XMQuaternionIdentity();
	}

	Quaternion::Quaternion(float x, float y, float z, float w)
		: X(_dxQuaternion.m128_f32[0])
		  , Y(_dxQuaternion.m128_f32[1])
		  , Z(_dxQuaternion.m128_f32[2])
		  , W(_dxQuaternion.m128_f32[3])
	{
		_dxQuaternion = XMQuaternionIdentity();
		_dxQuaternion.m128_f32[0] = x;
		_dxQuaternion.m128_f32[1] = y;
		_dxQuaternion.m128_f32[2] = z;
		_dxQuaternion.m128_f32[3] = w;
	}

	Quaternion::Quaternion(const Quaternion& quaternion)
		: X(_dxQuaternion.m128_f32[0])
		  , Y(_dxQuaternion.m128_f32[1])
		  , Z(_dxQuaternion.m128_f32[2])
		  , W(_dxQuaternion.m128_f32[3])
	{
		_dxQuaternion = quaternion._dxQuaternion;
	}

	Quaternion::Quaternion(const XMVECTOR& quaternion)
		: X(_dxQuaternion.m128_f32[0])
		  , Y(_dxQuaternion.m128_f32[1])
		  , Z(_dxQuaternion.m128_f32[2])
		  , W(_dxQuaternion.m128_f32[3])
	{
		_dxQuaternion = quaternion;
	}

	void Quaternion::SetEulerAngles(float pitch, float yaw, float roll)
	{
		pitch = XMConvertToRadians(pitch);
		yaw = XMConvertToRadians(yaw);
		roll = XMConvertToRadians(roll);
		_dxQuaternion = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	}

	void Quaternion::SetEulerAngles(const Vector3& eulerAngles)
	{
		eulerAngles.X = XMConvertToRadians(eulerAngles.X);
		eulerAngles.Y = XMConvertToRadians(eulerAngles.Y);
		eulerAngles.Z = XMConvertToRadians(eulerAngles.Z);
		_dxQuaternion = XMQuaternionRotationRollPitchYawFromVector(eulerAngles);
	}

	Vector3 Quaternion::GetEulerAngles() const
	{
		XMFLOAT4 q;
		XMStoreFloat4(&q, _dxQuaternion);

		XMFLOAT3 angles;
		angles.x = float(atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * (q.y * q.y) - 2 * (q.z * q.z)));
		angles.y = float(asin(2 * q.x * q.y + 2 * q.z * q.w));
		angles.z = float(atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * (q.x * q.x) - 2 * (q.z * q.z)));

		return angles;
	}

	Quaternion::operator DirectX::XMFLOAT4() const
	{
		XMFLOAT4 quat;
		XMStoreFloat4(&quat, _dxQuaternion);
		return quat;
	}

	Quaternion::operator DirectX::XMVECTOR() const
	{
		return _dxQuaternion;
	}

	Vector3 Quaternion::operator*(const Vector3& rhs) const
	{
		return XMVector3Rotate(rhs, _dxQuaternion);
	}

	Quaternion Quaternion::operator*(const Quaternion& rhs) const
	{
		return XMQuaternionMultiply(_dxQuaternion, rhs);
	}

	void Quaternion::operator=(const Quaternion& rhs)
	{
		_dxQuaternion = rhs._dxQuaternion;
	}
}

