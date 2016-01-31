#pragma once

#include "../../../stdafx.h"

namespace Engine
{
	class VertexBufferInstance;
}

namespace EngineNET
{
	ref class Vector2;
	ref class Vector3;
	ref class Colour;

	public ref struct Vertex
	{
		Vertex(Vector3^ pos, Colour^ col, Vector2^ uv);
		Vertex() {};

		Vector3^ Pos;
		Colour^ Col;
		Vector3^ Uv;
	};

	public ref class VertexBufferInstance
	{
	public:
		VertexBufferInstance();
		VertexBufferInstance(Engine::VertexBufferInstance* instance);
		operator Engine::VertexBufferInstance*();

		void SetVertices(System::Collections::Generic::List<Vertex^>^ vertices);
		System::Collections::Generic::List<Vertex^>^ GetVertices();
		int Count();

	private:
		Engine::VertexBufferInstance* _pVertexBufferInstance;
	};
}