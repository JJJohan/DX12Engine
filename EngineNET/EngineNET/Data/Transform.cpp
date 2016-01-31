namespace EngineNET
{
	Transform::Transform(Engine::Transform* transform)
	{
		_pTransform = transform;
	}

	void Transform::Move(Vector3^ translation, Space relativeTo)
	{
		Engine::Vector3* v = new Engine::Vector3(translation->X, translation->Y, translation->Z);
		_pTransform->Move(*v, (Engine::Space)relativeTo);
		delete v;
	}

	void Transform::Rotate(Vector3^ eulerAngles)
	{
		_pTransform->Rotate(*(Engine::Vector3*)eulerAngles);
	}

	void Transform::Rotate(float pitch, float yaw, float roll)
	{
		_pTransform->Rotate(pitch, yaw, roll);
	}

	void Transform::LookAt(float x, float y, float z)
	{
		_pTransform->LookAt(x, y, z);
	}

	void Transform::LookAt(Vector3^ position)
	{
		_pTransform->LookAt(*(Engine::Vector3*)position);
	}

#pragma managed(push, off)
	float* GetPos(Engine::Transform* transform)
	{
		Engine::Vector3 vec = transform->GetPosition();
		return new float[3] { vec.X, vec.Y, vec.Z };
	}

	float* GetRot(Engine::Transform* transform)
	{
		Engine::Quaternion q = transform->GetRotation();
		return new float[4]{ q.X, q.Y, q.Z, q.W };
	}

	float* GetScale(Engine::Transform* transform)
	{
		Engine::Vector3 vec = transform->GetScale();
		return new float[3]{ vec.X, vec.Y, vec.Z };
	}
#pragma managed(pop)

	Vector3^ Transform::Position::get()
	{
		Engine::Vector3* v = new Engine::Vector3();

		float* out = GetPos(_pTransform);
		Vector3^ outVec = gcnew Vector3(out[0], out[1], out[2]);
		delete[] out;

		return outVec;
	}

	void Transform::Position::set(Vector3^ v)
	{
		_pTransform->SetPosition(v->X, v->Y, v->Z);
	}

	Quaternion^ Transform::Rotation::get()
	{
		Engine::Vector3* v = new Engine::Vector3();

		float* out = GetRot(_pTransform);
		Quaternion^ outQ = gcnew Quaternion(out[0], out[1], out[2], out[3]);
		delete[] out;

		return outQ;
	}

	void Transform::Rotation::set(Quaternion^ v)
	{
		_pTransform->SetRotation(*(Engine::Quaternion*)v);
	}

	Vector3^ Transform::Scale::get()
	{
		Engine::Vector3* v = new Engine::Vector3();

		float* out = GetScale(_pTransform);
		Vector3^ outVec = gcnew Vector3(out[0], out[1], out[2]);
		delete[] out;

		return outVec;
	}

	void Transform::Scale::set(Vector3^ v)
	{
		_pTransform->SetPosition(v->X, v->Y, v->Z);
	}
}