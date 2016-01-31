#include "../../../stdafx.h"
#include "../../../Data/Colour.h"
#include "../../../Data/Vector2.h"
#include "../../../Data/Vector3.h"
#include "VertexBufferInstance.h"
#include "Engine/Rendering/DX12/VertexBufferInstance.h"
#include <vector>

namespace EngineNET
{
	Vertex::Vertex(Vector3^ pos, Colour^ col, Vector2^ uv)
	{
		Pos = pos;
		Col = col;
		Uv = gcnew Vector3(uv->X, uv->Y, 0.0f);
	}

	VertexBufferInstance::VertexBufferInstance()
	{
		_pVertexBufferInstance = new Engine::VertexBufferInstance();
	}

	VertexBufferInstance::VertexBufferInstance(Engine::VertexBufferInstance* instance)
	{
		_pVertexBufferInstance = instance;
	}

	VertexBufferInstance::operator Engine::VertexBufferInstance*()
	{
		return _pVertexBufferInstance;
	}

#pragma managed(push, off)
	void _SetVertices(Engine::VertexBufferInstance* vertexBuffer, std::vector<std::vector<float>>& verts, int count)
	{
		std::vector<float> pos = verts[0];
		std::vector<float> col = verts[1];
		std::vector<float> uv = verts[2];

		std::vector<Engine::Vertex> vertices(count);
		for (int i = 0, posIdx = 0, colIdx = 0; i < count; ++i, posIdx += 3, colIdx += 4)
		{
			Engine::Vertex v;
			v.Pos = Engine::Vector3(pos[posIdx], pos[posIdx + 1], pos[posIdx + 2]);
			v.Col = Engine::Colour(col[colIdx], col[colIdx + 1], col[colIdx + 2], col[colIdx + 3]);
			v.Uv = Engine::Vector3(uv[posIdx], uv[posIdx + 1], uv[posIdx + 2]);
			vertices[i] = v;
		}

		vertexBuffer->SetVertices(vertices);
	}
#pragma managed(pop)

	void VertexBufferInstance::SetVertices(System::Collections::Generic::List<Vertex^>^ vertices)
	{
		int count = vertices->Count;
		std::vector<std::vector<float>> vec(3);
		vec[0] = std::vector<float>(count * 3);
		vec[1] = std::vector<float>(count * 4);
		vec[2] = std::vector<float>(count * 3);
		for (int i = 0, posIdx = 0, colIdx = 0; i < count; ++i, posIdx += 3, colIdx +=4)
		{
			vec[0][posIdx] = vertices[i]->Pos->X;
			vec[0][posIdx + 1] = vertices[i]->Pos->Y;
			vec[0][posIdx + 2] = vertices[i]->Pos->Z;

			vec[1][colIdx] = vertices[i]->Col->R;
			vec[1][colIdx + 1] = vertices[i]->Col->G;
			vec[1][colIdx + 2] = vertices[i]->Col->B;
			vec[1][colIdx + 3] = vertices[i]->Col->A;

			vec[2][posIdx] = vertices[i]->Uv->X;
			vec[2][posIdx + 1] = vertices[i]->Uv->Y;
			vec[2][posIdx + 2] = vertices[i]->Uv->Z;
		}

		_SetVertices(_pVertexBufferInstance, vec, count);
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

			vertices[i] = v;
		}

		return vertices;
	}

	int VertexBufferInstance::Count()
	{
		return int(_pVertexBufferInstance->Count());
	}
}