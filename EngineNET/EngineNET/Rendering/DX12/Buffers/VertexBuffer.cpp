#include "../../../stdafx.h"

#include "VertexBuffer.h"
#include "Engine/Rendering/DX12/VertexBuffer.h"
#include <vector>

namespace EngineNET
{
	VertexBufferInstance::VertexBufferInstance()
	{
		_pVertexBufferInstance = new Engine::VertexBufferInstance();
	}

	VertexBufferInstance::~VertexBufferInstance()
	{
		this->!VertexBufferInstance();
	}

	VertexBufferInstance::!VertexBufferInstance()
	{
		delete _pVertexBufferInstance;
	}

	void VertexBufferInstance::SetVertices(System::Collections::Generic::List<Vertex^>^ vertices)
	{
		int count = vertices->Count;
		std::vector<Vertex^> vec(count);
		for (int i = 0; i < count; ++i)
		{
			vec.push_back(vertices[i]);
		}

		_pVertexBufferInstance->SetVertices(vec);
	}

	System::Collections::Generic::List<Vertex^>^ VertexBufferInstance::GetVertices()
	{
		std::vector<Vertex^> vec = _pVertexBufferInstance->GetVertices();
		int count = vec.size();
		System::Collections::Generic::List<Vertex^>^ vertices = gcnew System::Collections::Generic::List<Vertex^>(count);
		for (int i = 0; i < count; ++i)
		{
			vertices->Add(vec[0]);
		}

		return vertices;
	}
}