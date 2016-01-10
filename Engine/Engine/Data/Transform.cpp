#include "Transform.h"

using namespace DirectX;

namespace Engine
{
	Transform::Transform()
		: Moved(false)
	{
		_scale = Vector3::One;
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		_position.X = x;
		_position.Y = y;
		_position.Z = z;
		UpdateMatrix();
	}

	void Transform::SetPosition(const Vector3& position)
	{
		_position = position;
		UpdateMatrix();
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		_rotation = rotation;
		UpdateMatrix();
	}

	void Transform::SetScale(float x, float y, float z)
	{
		_scale.X = x;
		_scale.Y = y;
		_scale.Z = z;
		UpdateMatrix();
	}

	void Transform::SetScale(const Vector3& scale)
	{
		_scale = scale;
		UpdateMatrix();
	}

	const Vector3& Transform::GetPosition() const
	{
		return _position;
	}

	const Quaternion& Transform::GetRotation() const
	{
		return _rotation;
	}

	const Vector3& Transform::GetScale() const
	{
		return _scale;
	}

	const XMMATRIX& Transform::GetMatrix() const
	{
		return _worldMatrix;
	}

	void Transform::UpdateMatrix()
	{
		_worldMatrix = XMMatrixTransformation(Vector3::Zero, Quaternion::Identity, _scale, Vector3::Zero, _rotation, _position);
		Moved = true;
	}

	void Transform::Move(const Vector3& translation, Space relativeTo)
	{
		Vector3 t = translation;
		if (relativeTo == Local)
		{
			t = Vector3(XMVector3Rotate(t, _rotation));
		}

		_position = _position + t;
		UpdateMatrix();
	}

	void Transform::Rotate(const Vector3& eulerAngles)
	{
		float pitch = XMConvertToRadians(eulerAngles.X);
		float yaw = XMConvertToRadians(eulerAngles.Y);
		float roll = XMConvertToRadians(eulerAngles.Z);

		XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
		_rotation = XMQuaternionMultiply(rotation, _rotation);

		UpdateMatrix();
	}

	void Transform::Rotate(float pitch, float yaw, float roll)
	{
		Rotate(XMFLOAT3(pitch, yaw, roll));
	}

}
