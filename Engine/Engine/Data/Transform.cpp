using namespace DirectX;

namespace Engine
{
	Transform::Transform()
		: Moved(false)
		, _setPosOverride(false)
		, _setRotOverride(false)
		, _setScaleOverride(false)
		, _getPosOverride(false)
		, _getRotOverride(false)
		, _getScaleOverride(false)
	{
		_worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		_scale = Vector3::One;
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		if (_setPosOverride)
		{
			_position = _setPosOverrideFunc(x, y, z);
		}
		else
		{
			_position.X = x;
			_position.Y = y;
			_position.Z = z;
		}

		UpdateMatrix();
	}

	void Transform::SetPosition(const Vector3& position)
	{
		if (_setPosOverride)
		{
			_position = _setPosOverrideFunc(position.X, position.Y, position.Z);
		}
		else
		{
			_position = position;
		}

		UpdateMatrix();
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		if (_setRotOverride)
		{
			_rotation = _setRotOverrideFunc(rotation.X, rotation.Y, rotation.Z, rotation.W);
		}
		else
		{
			_rotation = rotation;
		}

		UpdateMatrix();
	}

	void Transform::SetScale(float x, float y, float z)
	{
		if (_setScaleOverride)
		{
			_scale = _setScaleOverrideFunc(x, y, z);
		}
		else
		{
			_scale.X = x;
			_scale.Y = y;
			_scale.Z = z;
		}

		UpdateMatrix();
	}

	void Transform::SetScale(const Vector3& scale)
	{
		if (_setScaleOverride)
		{
			_scale = _setScaleOverrideFunc(scale.X, scale.Y, scale.Z);
		}
		else
		{
			_scale = scale;
		}

		UpdateMatrix();
	}

	Vector3 Transform::GetPosition() const
	{
		if (_getPosOverride)
		{
			return _getPosOverrideFunc(_position.X, _position.Y, _position.Z);
		}

		return _position;
	}

	Quaternion Transform::GetRotation() const
	{
		if (_getRotOverride)
		{
			return _getRotOverrideFunc(_rotation.X, _rotation.Y, _rotation.Z, _rotation.W);
		}

		return _rotation;
	}

	Vector3 Transform::GetScale() const
	{
		if (_getScaleOverride)
		{
			return _getScaleOverrideFunc(_scale.X, _scale.Y, _scale.Z);
		}

		return _scale;
	}

	void Transform::SetPosOverride(std::function<Vector3(float, float, float)> func)
	{
		_setPosOverride = true;
		_setPosOverrideFunc = func;
	}

	void Transform::SetRotOverride(std::function<Quaternion(float, float, float, float)> func)
	{
		_setRotOverride = true;
		_setRotOverrideFunc = func;
	}

	void Transform::SetScaleOverride(std::function<Vector3(float, float, float)> func)
	{
		_setScaleOverride = true;
		_setScaleOverrideFunc = func;
	}

	void Transform::GetPosOverride(std::function<Vector3(float, float, float)> func)
	{
		_getPosOverride = true;
		_getPosOverrideFunc = func;
	}

	void Transform::GetRotOverride(std::function<Quaternion(float, float, float, float)> func)
	{
		_getRotOverride = true;
		_getRotOverrideFunc = func;
	}

	void Transform::GetScaleOverride(std::function<Vector3(float, float, float)> func)
	{
		_getScaleOverride = true;
		_getScaleOverrideFunc = func;
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
		_eulerAngles += eulerAngles;
		_eulerAngles.X = Math::Clamp(_eulerAngles.X, -90.0f, 90.0f);

		float pitch = XMConvertToRadians(_eulerAngles.X);
		float yaw = XMConvertToRadians(_eulerAngles.Y);
		float roll = XMConvertToRadians(_eulerAngles.Z);

		XMVECTOR pitchRot = XMQuaternionRotationAxis(Vector3::Right, pitch);
		XMVECTOR yawRot = XMQuaternionRotationAxis(Vector3::Up, yaw);
		XMVECTOR rollRot = XMQuaternionRotationAxis(Vector3::Forward, roll);
		_rotation = XMQuaternionMultiply(XMQuaternionMultiply(pitchRot, yawRot), rollRot);

		UpdateMatrix();
	}

	void Transform::LookAt(float x, float y, float z)
	{
		LookAt(Vector3(x, y, z));
	}

	Vector3 Transform::GetEulerAngles() const
	{
		return _eulerAngles;
	}

	void Transform::LookAt(const Vector3& position)
	{
		XMMATRIX lookAt = XMMatrixLookAtLH(_position, position, Vector3::Up);
		_rotation = XMQuaternionRotationMatrix(lookAt);

		UpdateMatrix();
	}

	void Transform::Rotate(float pitch, float yaw, float roll)
	{
		Rotate(XMFLOAT3(pitch, yaw, roll));
	}
}

