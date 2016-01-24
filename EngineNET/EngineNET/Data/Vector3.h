#pragma once

#include "../stdafx.h"

namespace Engine
{
	class Vector3;
}

namespace EngineNET
{
	ref class Vector2;

	public ref class Vector3
	{
	public:
		Vector3();
		~Vector3();
		!Vector3();

		Vector3(Vector2^ rhs);
		Vector3(Vector3^ rhs);
		Vector3(float x, float y, float z);
		Vector3(int x, int y, int z);
		Vector3(Engine::Vector3& vec);

		Vector3^ operator+(Vector3^ rhs);
		Vector3^ operator*(Vector3^ rhs);
		Vector3^ operator-(Vector3^ rhs);
		Vector3^ operator/(Vector3^ rhs);
		Vector3^ operator*(float rhs);
		Vector3^ operator/(float rhs);
		void operator=(Vector3^ rhs);
		operator Engine::Vector3*();

		Vector3^ operator+=(Vector3^ rhs);
		Vector3^ operator*=(Vector3^ rhs);
		Vector3^ operator-=(Vector3^ rhs);
		Vector3^ operator/=(Vector3^ rhs);
		Vector3^ operator*=(float rhs);
		Vector3^ operator/=(float rhs);

		static initonly Vector3^ Zero;
		static initonly Vector3^ One;
		static initonly Vector3^ Up;
		static initonly Vector3^ Forward;
		static initonly Vector3^ Right;

		property float X { float get(); void set(float v); };
		property float Y { float get(); void set(float v); };
		property float Z { float get(); void set(float v); };

	private:
		static Vector3();

		Engine::Vector3* _pVector3;
	};
}