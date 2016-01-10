#pragma once

#include "Vector3.h"
#include "Quaternion.h"

namespace Engine
{
	enum Space
	{
		Local,
		World
	};

	class Transform
	{
	public:
		Transform();

		void SetPosition(float x, float y, float z);
		void SetPosition(const Vector3& position);
		void SetRotation(const Quaternion& rotation);
		void SetScale(float x, float y, float z);
		void SetScale(const Vector3& scale);

		void Move(const Vector3& translation, Space relativeTo = World);
		void Rotate(const Vector3& eulerAngles);
		void Rotate(float pitch, float yaw, float roll);

		const Vector3& GetPosition() const;
		const Quaternion& GetRotation() const;
		const Vector3& GetScale() const;
		const DirectX::XMMATRIX& GetMatrix() const;

		bool Moved;

	private:
		void UpdateMatrix();

		Vector3 _position;
		Quaternion _rotation;
		Vector3 _scale;

		DirectX::XMMATRIX _worldMatrix;
	};
}

