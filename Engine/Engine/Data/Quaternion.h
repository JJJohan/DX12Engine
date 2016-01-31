#pragma once

namespace Engine
{
	class Vector3;

	class Quaternion
	{
	public:
		ENGINE_API Quaternion();
		ENGINE_API Quaternion(float x, float y, float z, float w);
		ENGINE_API Quaternion(const Quaternion& quaternion);
		ENGINE_API Quaternion(const DirectX::XMVECTOR& quaternion);

		static ENGINE_API Quaternion Euler(float pitch, float yaw, float roll);
		static ENGINE_API Quaternion Euler(const Vector3& eulerAngles);
		ENGINE_API Vector3 GetEulerAngles() const;

		ENGINE_API Vector3 operator*(const Vector3& rhs) const;
		ENGINE_API Quaternion operator*(const Quaternion& rhs) const;
		ENGINE_API void operator=(const Quaternion& rhs);

		ENGINE_API operator DirectX::XMFLOAT4() const;
		ENGINE_API operator DirectX::XMVECTOR() const;

		static ENGINE_API const Quaternion& Identity;

		float& X;
		float& Y;
		float& Z;
		float& W;

	private:
		DirectX::XMVECTOR _dxQuaternion;
	};
}

