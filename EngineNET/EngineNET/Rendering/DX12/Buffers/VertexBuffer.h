#pragma once

#include "../../../stdafx.h"

namespace Engine
{
	class VertexBufferInstance;
}

namespace EngineNET
{
	ref class Vector3;
	ref class Vector4;

	public ref struct Vertex
	{
		Vector3^ Pos;
		Vector4^ Col;
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