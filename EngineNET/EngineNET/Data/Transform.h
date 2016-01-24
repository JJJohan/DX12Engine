#pragma once

#include "../stdafx.h"

namespace Engine
{
	class Transform;
}

namespace EngineNET
{
	ref class Vector3;
	ref class Quaternion;

	enum Space
	{
		Local = 0,
		World = 1
	};

	public ref class Transform
	{
	public:
		Transform();
		~Transform();
		!Transform();

		void Move(Vector3^ translation, Space relativeTo);

		property Vector3^ Position { Vector3^ get(); void set(Vector3^ v); }
		property Quaternion^ Rotation { Quaternion^ get(); void set(Quaternion^ v); }
		property Vector3^ Scale { Vector3^ get(); void set(Vector3^ v); }

	private:
		Engine::Transform* _pTransform;
	};
}