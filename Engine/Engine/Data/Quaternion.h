#pragma once
#include <DirectXMath.h>

namespace Engine
{
	class Vector3;

	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(float x, float y, float z, float w);
		Quaternion(const Quaternion& quaternion);
		Quaternion(const DirectX::XMVECTOR& quaternion);

		void SetEulerAngles(float pitch, float yaw, float roll);
		void SetEulerAngles(const Vector3& eulerAngles);
		Vector3 GetEulerAngles() const;

		Vector3 operator*(const Vector3& rhs) const;
		Quaternion operator*(const Quaternion& rhs) const;
		void operator=(const Quaternion& rhs);

		operator DirectX::XMFLOAT4() const;
		operator DirectX::XMVECTOR() const;

		static const Quaternion& Identity;

		float& X;
		float& Y;
		float& Z;
		float& W;

	private:
		DirectX::XMVECTOR _dxQuaternion;
	};
}

