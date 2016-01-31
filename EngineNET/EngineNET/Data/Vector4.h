#pragma once

namespace Engine
{
	class Vector4;
}

namespace EngineNET
{
	ref class Vector3;

	public ref class Vector4
	{
	public:
		Vector4();
		~Vector4();
		!Vector4();

		Vector4(Vector3^ rhs);
		Vector4(Vector4^ rhs);
		Vector4(float x, float y, float z, float w);
		Vector4(int x, int y, int z, int w);
		Vector4(Engine::Vector4& vec);

		Vector4^ operator+(Vector4^ rhs);
		Vector4^ operator*(Vector4^ rhs);
		Vector4^ operator-(Vector4^ rhs);
		Vector4^ operator/(Vector4^ rhs);
		Vector4^ operator*(float rhs);
		Vector4^ operator/(float rhs);
		void operator=(Vector4^ rhs);
		operator Engine::Vector4*();

		Vector4^ operator+=(Vector4^ rhs);
		Vector4^ operator*=(Vector4^ rhs);
		Vector4^ operator-=(Vector4^ rhs);
		Vector4^ operator/=(Vector4^ rhs);
		Vector4^ operator*=(float rhs);
		Vector4^ operator/=(float rhs);

		static initonly Vector4^ Zero;
		static initonly Vector4^ One;

		property float X { float get(); void set(float v); };
		property float Y { float get(); void set(float v); };
		property float Z { float get(); void set(float v); };
		property float W { float get(); void set(float v); };

	private:
		static Vector4();

		Engine::Vector4* _pVector4;
	};
}