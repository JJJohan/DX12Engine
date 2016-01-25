#pragma once

#include "../stdafx.h"

namespace Engine
{
	class Colour;
}

namespace EngineNET
{
	public ref class Colour
	{
	public:
		Colour();
		Colour(float r, float g, float b);
		Colour(float r, float g, float b, float a);
		Colour(Engine::Colour& col);
		~Colour();
		!Colour();

		Colour^ operator*(Colour^ rhs);
		Colour^ operator+(Colour^ rhs);
		Colour^ operator-(Colour^ rhs);

		Colour^ operator*=(Colour^ rhs);
		Colour^ operator+=(Colour^ rhs);
		Colour^ operator-=(Colour^ rhs);
		operator Engine::Colour*();

		property float R { float get(); void set(float v); };
		property float G { float get(); void set(float v); };
		property float B { float get(); void set(float v); };
		property float A { float get(); void set(float v); };

		static initonly Colour^ Clear;
		static initonly Colour^ White;
		static initonly Colour^ Red;
		static initonly Colour^ Green;
		static initonly Colour^ Blue;
		static initonly Colour^ Yellow;
		static initonly Colour^ Pink;
		static initonly Colour^ Black;

	private:
		static Colour();
		Engine::Colour* _pColour;
	};
}