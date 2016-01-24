#include "Quaternion.h"
#include "Vector3.h"
#include "Engine/Data/Quaternion.h"
#include "Engine/Data/Vector3.h"

namespace EngineNET
{
	static Quaternion::Quaternion()
	{
		Identity = gcnew Quaternion();
	}

	Quaternion::Quaternion()
	{
		_pQuaternion = new Engine::Quaternion();
	}

	Quaternion::~Quaternion()
	{
		this->!Quaternion();
	}

	Quaternion::!Quaternion()
	{
		delete _pQuaternion;
	}

	Quaternion::Quaternion(float x, float y, float z, float w)
	{
		_pQuaternion = new Engine::Quaternion(x, y, z, w);
	}

	void Quaternion::SetEulerAngles(float pitch, float yaw, float roll)
	{
		_pQuaternion->SetEulerAngles(pitch, yaw, roll);
	}

	void Quaternion::SetEulerAngles(Vector3^ eulerAngles)
	{
		_pQuaternion->SetEulerAngles(*(Engine::Vector3*)eulerAngles);
	}

	Quaternion::Quaternion(Quaternion^ q)
	{
		_pQuaternion = new Engine::Quaternion();
		_pQuaternion->X = q->_pQuaternion->X;
		_pQuaternion->Y = q->_pQuaternion->Y;
		_pQuaternion->Z = q->_pQuaternion->Z;
		_pQuaternion->W = q->_pQuaternion->W;
	}

	Quaternion::Quaternion(Engine::Quaternion& q)
	{
		_pQuaternion = new Engine::Quaternion();
		_pQuaternion->X = q.X;
		_pQuaternion->Y = q.Y;
		_pQuaternion->Z = q.Z;
		_pQuaternion->W = q.W;
	}

#pragma managed(push, off)
	Engine::Quaternion* Multiply(Engine::Quaternion* lhs, Engine::Quaternion* rhs)
	{
		Engine::Quaternion tmp;
		tmp = lhs->operator*(*rhs);

		return new Engine::Quaternion(tmp.X, tmp.Y, tmp.Z, tmp.W);
	}

	Engine::Vector3* Multiply(Engine::Quaternion* lhs, Engine::Vector3* rhs)
	{
		Engine::Vector3 tmp;
		tmp = lhs->operator*(*rhs);

		return new Engine::Vector3(tmp.X, tmp.Y, tmp.Z);
	}

	Engine::Vector3* Euler(Engine::Quaternion* q)
	{
		Engine::Vector3 tmp = q->GetEulerAngles();
		return new Engine::Vector3(tmp.X, tmp.Y, tmp.Z);
	}
#pragma managed(pop)

	Vector3^ Quaternion::GetEulerAngles()
	{
		Engine::Vector3* euler = Euler(_pQuaternion);
		Vector3^ out = gcnew Vector3(euler->X, euler->Y, euler->Z);
		delete euler;
		return out;
	}

	Quaternion::operator Engine::Quaternion*()
	{
		return _pQuaternion;
	}

	Vector3^ Quaternion::operator*(Vector3^ rhs)
	{
		Engine::Vector3* vec = Multiply(_pQuaternion, rhs);
		Vector3^ out = gcnew Vector3(vec->X, vec->Y, vec->Z);
		delete vec;

		return out;
	}

	Quaternion^ Quaternion::operator*(Quaternion^ rhs)
	{
		Engine::Quaternion* q = Multiply(_pQuaternion, rhs);
		Quaternion^ out = gcnew Quaternion(q->X, q->Y, q->Z, q->W);
		delete q;

		return out;
	}

	void Quaternion::operator=(Quaternion^ rhs)
	{
		delete _pQuaternion;
		_pQuaternion = new Engine::Quaternion();
		_pQuaternion->X = rhs->_pQuaternion->X;
		_pQuaternion->Y = rhs->_pQuaternion->Y;
		_pQuaternion->Z = rhs->_pQuaternion->Z;
		_pQuaternion->W = rhs->_pQuaternion->W;
	}

	float Quaternion::X::get()
	{
		return _pQuaternion->X;
	}

	void Quaternion::X::set(float v)
	{
		_pQuaternion->X = v;
	}

	float Quaternion::Y::get()
	{
		return _pQuaternion->Y;
	}

	void Quaternion::Y::set(float v)
	{
		_pQuaternion->Y = v;
	}

	float Quaternion::Z::get()
	{
		return _pQuaternion->Z;
	}

	void Quaternion::Z::set(float v)
	{
		_pQuaternion->Z = v;
	}

	float Quaternion::W::get()
	{
		return _pQuaternion->W;
	}

	void Quaternion::W::set(float v)
	{
		_pQuaternion->W = v;
	}
}