#pragma once

namespace Engine
{
	class Vector2;
}

namespace EngineNET
{
	public ref class Vector2
	{
	public:
		Vector2();
		~Vector2();
		!Vector2();

		Vector2(Vector2^ rhs);
		Vector2(float x, float y);
		Vector2(int x, int y);
		Vector2(Engine::Vector2& vec);

		Vector2^ operator+(Vector2^ rhs);
		Vector2^ operator*(Vector2^ rhs);
		Vector2^ operator-(Vector2^ rhs);
		Vector2^ operator/(Vector2^ rhs);
		Vector2^ operator*(float rhs);
		Vector2^ operator/(float rhs);
		void operator=(Vector2^ rhs);
		operator Engine::Vector2*();

		Vector2^ operator+=(Vector2^ rhs);
		Vector2^ operator*=(Vector2^ rhs);
		Vector2^ operator-=(Vector2^ rhs);
		Vector2^ operator/=(Vector2^ rhs);
		Vector2^ operator*=(float rhs);
		Vector2^ operator/=(float rhs);

		static initonly Vector2^ Zero;
		static initonly Vector2^ One;
		static initonly Vector2^ Up;
		static initonly Vector2^ Down;
		static initonly Vector2^ Left;
		static initonly Vector2^ Right;

		property float X { float get(); void set(float v); };
		property float Y { float get(); void set(float v); };

	private:
		static Vector2();

		Engine::Vector2* _pVector2;
	};
}