#pragma once

#include "../../../stdafx.h"

namespace Engine
{
	class VertexBufferInstance;
}

namespace EngineNET
{
	ref class Vector3;
	ref class Colour;

	public ref struct Vertex
	{
		Vector3^ Pos;
		Colour^ Col;
		Vector3^ Uv;
	};

	public ref class VertexBufferInstance
	{
	public:
		VertexBufferInstance();
		~VertexBufferInstance();
		!VertexBufferInstance();

		void SetVertices(System::Collections::Generic::List<Vertex^>^ vertices);
		System::Collections::Generic::List<Vertex^>^ GetVertices();

	private:
		Engine::VertexBufferInstance* _pVertexBufferInstance;
	};
}