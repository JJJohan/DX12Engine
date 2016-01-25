#include "../../../stdafx.h"

#include "../../../Data/Colour.h"
#include "../../../Data/Vector3.h"
#include "VertexBufferInstance.h"
#include "Engine/Rendering/DX12/VertexBufferInstance.h"
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

#pragma managed(push, off)
	void _SetVertices(Engine::VertexBufferInstance* vertexBuffer, std::vector<std::vector<float>*>* verts)
	{
		std::vector<std::vector<float>*> vertexArray = *verts;
		std::vector<float> pos = *vertexArray[0];
		std::vector<float> col = *vertexArray[1];
		std::vector<float> uv = *vertexArray[2];

		int size = int(verts->size());
		std::vector<Engine::Vertex> vertices(size);
		for (int i = 0; i < size; ++i)
		{
			Engine::Vertex v;
			v.Pos = Engine::Vector3(pos[i], pos[i + 1], pos[i + 2]);
			v.Col = Engine::Colour(col[i], col[i + 1], col[i + 2]);
			v.Uv = Engine::Vector3(uv[i], uv[i + 1], uv[i + 2]);
			vertices.push_back(v);
		}

		vertexBuffer->SetVertices(vertices);

		delete vertexArray[0];
		delete vertexArray[1];
		delete vertexArray[2];
		delete verts;
	}
#pragma managed(pop)

	void VertexBufferInstance::SetVertices(System::Collections::Generic::List<Vertex^>^ vertices)
	{
		int count = vertices->Count;
		std::vector<std::vector<float>*>* vec = new std::vector<std::vector<float>*>(3);
		vec->push_back(new std::vector<float>(count));
		vec->push_back(new std::vector<float>(count));
		vec->push_back(new std::vector<float>(count));
		for (int i = 0; i < count; ++i)
		{
			vec->at(0)->push_back(vertices[i]->Pos->Z);
			vec->at(0)->push_back(vertices[i]->Pos->Y);
			vec->at(0)->push_back(vertices[i]->Pos->X);

			vec->at(1)->push_back(vertices[i]->Col->R);
			vec->at(1)->push_back(vertices[i]->Col->G);
			vec->at(1)->push_back(vertices[i]->Col->B);
			vec->at(1)->push_back(vertices[i]->Col->A);

			vec->at(2)->push_back(vertices[i]->Uv->X);
			vec->at(2)->push_back(vertices[i]->Uv->Y);
			vec->at(2)->push_back(vertices[i]->Uv->Z);
		}

		_SetVertices(_pVertexBufferInstance, vec);
	}

	System::Collections::Generic::List<Vertex^>^ VertexBufferInstance::GetVertices()
	{
		std::vector<Engine::Vertex> vec = _pVertexBufferInstance->GetVertices();
		int count = int(vec.size());
		System::Collections::Generic::List<Vertex^>^ vertices = gcnew System::Collections::Generic::List<Vertex^>(count);
		for (int i = 0; i < count; ++i)
		{
			Vertex^ v = gcnew Vertex();
			v->Pos = gcnew Vector3(vec[i].Pos.x, vec[i].Pos.y, vec[i].Pos.z);
			v->Col = gcnew Colour(vec[i].Col.x, vec[i].Col.y, vec[i].Col.z, vec[i].Col.w);
			v->Uv = gcnew Vector3(vec[i].Uv.x, vec[i].Uv.y, vec[i].Uv.z);

			vertices->Add(v);
		}

		return vertices;
	}
}