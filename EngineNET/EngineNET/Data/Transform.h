#pragma once

namespace Engine
{
	class Transform;
}

namespace EngineNET
{
	ref class Vector3;
	ref class Quaternion;

	public enum class Space
	{
		Local = 0,
		World = 1
	};

	public ref class Transform
	{
	public:
		Transform(Engine::Transform* transform);

		void Move(Vector3^ translation, Space relativeTo);
		void Rotate(Vector3^ eulerAngles);
		void Rotate(float pitch, float yaw, float roll);
		void LookAt(float x, float y, float z);
		void LookAt(Vector3^ position);

		property Vector3^ Position { Vector3^ get(); void set(Vector3^ v); }
		property Quaternion^ Rotation { Quaternion^ get(); void set(Quaternion^ v); }
		property Vector3^ Scale { Vector3^ get(); void set(Vector3^ v); }

	private:
		Engine::Transform* _pTransform;
	};
}