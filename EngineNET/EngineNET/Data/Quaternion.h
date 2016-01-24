#pragma once

#include "../stdafx.h"

namespace Engine
{
	class Quaternion;
}

namespace EngineNET
{
	ref class Vector3;

	public ref class Quaternion
	{
	public:
		Quaternion();
		~Quaternion();
		!Quaternion();

		Quaternion(float x, float y, float z, float w);
		Quaternion(Quaternion^ quaternion);

		void SetEulerAngles(float pitch, float yaw, float roll);
		void SetEulerAngles(Vector3^ eulerAngles);
		Vector3^ GetEulerAngles();

		Vector3^ operator*(Vector3^ rhs);
		Quaternion^ operator*(Quaternion^ rhs);
		void operator=(Quaternion^ rhs);
		operator Engine::Quaternion*();

		static initonly Quaternion^ Identity;

		property float X { float get(); void set(float v); };
		property float Y { float get(); void set(float v); };
		property float Z { float get(); void set(float v); };
		property float W { float get(); void set(float v); };

	private:
		static Quaternion();
		Quaternion(Engine::Quaternion& vec);

		Engine::Quaternion* _pQuaternion;
	};
}