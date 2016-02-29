#include "Primitives.h"
#include "RenderObject.h"
#include "VertexBufferInstance.h"
#include "IndexBufferInstance.h"
#include "../../Data/Vector3.h"

namespace Engine
{
	ENGINE_API RenderObject* Primitives::CreateCube(Vector3 pos, Quaternion rot, Vector3 scale)
	{
		return CreateCube("Cube", pos, rot, scale);
	}

	ENGINE_API RenderObject* Primitives::CreateCube(std::string name, Vector3 pos, Quaternion rot, Vector3 scale)
	{
		// Vertex Buffer
		std::vector<Vertex> vertices(24);
		Vector3 tl, tr, bl, br;
		for (int i = 0, idx = 0; i < 6; ++i, idx += 4)
		{
			switch (i)
			{
			case 0: // Front
			case 2: // Back
				tl = Vector3(i == 0 ? -0.5f : 0.5f, 0.5f, i == 0 ? -0.5f : 0.5f);
				tr = Vector3(i == 0 ? 0.5f : -0.5f, 0.5f, i == 0 ? -0.5f : 0.5f);
				bl = Vector3(i == 0 ? -0.5f : 0.5f, -0.5f, i == 0 ? -0.5f : 0.5f);
				br = Vector3(i == 0 ? 0.5f : -0.5f, -0.5f, i == 0 ? -0.5f : 0.5f);
				break;
			case 1: // Left
			case 3: // Right
				tl = Vector3(i == 1 ? -0.5f : 0.5f, 0.5f, i == 1 ? 0.5f : -0.5f);
				tr = Vector3(i == 1 ? -0.5f : 0.5f, 0.5f, i == 1 ? -0.5f : 0.5f);
				bl = Vector3(i == 1 ? -0.5f : 0.5f, -0.5f, i == 1 ? 0.5f : -0.5f);
				br = Vector3(i == 1 ? -0.5f : 0.5f, -0.5f, i == 1 ? -0.5f : 0.5f);
				break;
			case 4: // Top
			case 5: // Bottom
				tl = Vector3(i == 4 ? -0.5f : 0.5f, i == 5 ? -0.5f : 0.5f, 0.5f);
				tr = Vector3(i == 4 ? 0.5f : -0.5f, i == 5 ? -0.5f : 0.5f, 0.5f);
				bl = Vector3(i == 4 ? -0.5f : 0.5f, i == 5 ? -0.5f : 0.5f, -0.5f);
				br = Vector3(i == 4 ? 0.5f : -0.5f, i == 5 ? -0.5f : 0.5f, -0.5f);
				break;
			}

			vertices[idx] = Vertex(tl, Colour::White, Vector2(0, 0));
			vertices[idx + 1] = Vertex(tr, Colour::White, Vector2(1, 0));
			vertices[idx + 2] = Vertex(bl, Colour::White, Vector2(0, 1));
			vertices[idx + 3] = Vertex(br, Colour::White, Vector2(1, 1));
		}
		VertexBufferInstance* vertexBuffer = ResourceFactory::CreateVertexBufferInstance();
		vertexBuffer->SetVertices(vertices);

		// Index Buffer
		std::vector<int> indices(36);
		for (int i = 0, idx = 0; i < 36; i += 6, idx += 4)
		{
			indices[i] = idx;
			indices[i + 1] = idx + 1;
			indices[i + 2] = idx + 2;

			indices[i + 3] = idx + 1;
			indices[i + 4] = idx + 3;
			indices[i + 5] = idx + 2;
		}
		IndexBufferInstance* indexBuffer = ResourceFactory::CreateIndexBufferInstance();
		indexBuffer->SetIndices(indices);

		// Render Object
		RenderObject* renderObject = new RenderObject(name);
		renderObject->SetVertexBuffer(vertexBuffer);
		renderObject->SetIndexBuffer(indexBuffer);

		// Transform
		renderObject->Transform.SetPosition(pos);
		renderObject->Transform.SetRotation(rot);
		renderObject->Transform.SetScale(scale);

		return renderObject;
	}

	ENGINE_API RenderObject* Primitives::CreateSphere(int samples, Vector3 pos, Quaternion rot, Vector3 scale)
	{
		return CreateSphere("Sphere", samples, pos, rot, scale);
	}

	ENGINE_API RenderObject* Primitives::CreateSphere(std::string name, int samples, Vector3 pos, Quaternion rot, Vector3 scale)
	{
		if (samples < 4)
		{
			samples = 4;
		}

		if (samples % 2 == 1)
		{
			++samples; // even number
		}

		// Vertex Buffer
		std::vector<Vertex> vertices((samples + 1) * (samples + 1));

		// Formula
		// (x, y, z) = (sin(Pi * m/M) cos(2Pi * n/N), sin(Pi * m/M) sin(2Pi * n/N), cos(Pi * m/M))

		int idx = 0;
		float pi = Math::Pi;
		for (int i = 0; i <= samples; ++i)
		{
			float vertical = float(i) / float(samples);
			for (int j = 0; j <= samples; ++j)
			{
				float horizontal = float(j) / float(samples);

				Vector3 point = Vector3(
					sinf(pi * vertical) * cosf(2 * pi * horizontal),
					sinf(pi * vertical) * sinf(2 * pi * horizontal),
					cosf(pi * vertical));

				vertices[idx++] = Vertex(point, Colour::White, Vector2(vertical, horizontal));
			}
		}

		VertexBufferInstance* vertexBuffer = ResourceFactory::CreateVertexBufferInstance();
		vertexBuffer->SetVertices(vertices);

		// Index Buffer
		idx = 0;
		std::vector<int> indices(samples * samples * 6);
		for (int i = 0; i < samples; ++i)
		{
			for (int j = 0; j < samples; ++j)
			{
				int first = (i * (samples + 1)) + j;
				int second = first + samples + 1;

				indices[idx++] = first;
				indices[idx++] = second;
				indices[idx++] = first + 1;

				indices[idx++] = second;
				indices[idx++] = second + 1;
				indices[idx++] = first + 1;
			}
		}
		IndexBufferInstance* indexBuffer = ResourceFactory::CreateIndexBufferInstance();
		indexBuffer->SetIndices(indices);

		// Render Object
		RenderObject* renderObject = new RenderObject(name);
		renderObject->SetVertexBuffer(vertexBuffer);
		renderObject->SetIndexBuffer(indexBuffer);

		// Transform
		renderObject->Transform.SetPosition(pos);
		renderObject->Transform.SetRotation(rot);
		renderObject->Transform.SetScale(scale);

		return renderObject;
	}

	ENGINE_API RenderObject* Primitives::CreateCylinder(int sides, Vector3 pos, Quaternion rot, Vector3 scale)
	{
		return CreateCylinder("Cylinder", sides, pos, rot, scale);
	}

	ENGINE_API RenderObject * Primitives::CreateCylinder(std::string name, int sides, Vector3 pos, Quaternion rot, Vector3 scale)
	{
		if (sides < 3)
		{
			sides = 3;
		}

		float stepTheta = 2.0f * Math::Pi / sides;
		float stepU = 1.0f / sides;
		int verticesPerCircle = sides + 1;
		int indicesPerCap = sides - 2;
		int firstSidePos = verticesPerCircle * 2;
		int vertexCount = verticesPerCircle * 4;

		int posIndex = 0;
		int uvIndex = 0;
		int triIndex = 0;

		std::vector<Vector3> positions(vertexCount);
		std::vector<Vector2> uvs(vertexCount);
		std::vector<int> indices((indicesPerCap + sides) * 6);

		int curTheta = 0;
		std::vector<float> halfCosThetas(verticesPerCircle);
		std::vector<float> halfSinThetas(verticesPerCircle);
		for (int i = 0; i < verticesPerCircle; ++i)
		{
			halfCosThetas[i] = cosf(curTheta) * 0.5f;
			halfSinThetas[i] = sinf(curTheta) * 0.5f;
			curTheta += stepTheta;
		}

		// Top cap
		for (int i = 0; i < verticesPerCircle; ++i)
		{
			positions[posIndex++] = Vector3(halfCosThetas[i], 0.5f, halfSinThetas[i]);
			uvs[uvIndex++] = Vector2(halfCosThetas[i] + 0.5f, halfSinThetas[i] + 0.5);
		}
		for (int i = 0; i < indicesPerCap; ++i)
		{
			indices[triIndex++] = 0;
			indices[triIndex++] = i + 1;
			indices[triIndex++] = i + 2;
		}

		// Bottom cap
		for (int i = 0; i < verticesPerCircle; ++i)
		{
			positions[posIndex++] = Vector3(halfCosThetas[i], -0.5f, halfSinThetas[i]);
			uvs[uvIndex++] = Vector2(halfCosThetas[i] + 0.5f, -halfSinThetas[i] + 0.5f);
		}
		for (int i = 0; i < indicesPerCap; ++i)
		{
			indices[triIndex++] = verticesPerCircle + i + 2;
			indices[triIndex++] = verticesPerCircle + i + 1;
			indices[triIndex++] = verticesPerCircle;
		}

		// Top cap (for the sides)
		float curU = 1.0f;
		for (int i = 0; i < verticesPerCircle; ++i)
		{
			positions[posIndex++] = Vector3(halfCosThetas[i], 0.5f, halfSinThetas[i]);
			uvs[uvIndex++] = Vector2(curU, 0.0f);

			curU -= stepU;
		}

		// Bottom cap (for the sides)
		curU = 1.0f;
		for (int i = 0; i < verticesPerCircle; ++i)
		{
			positions[posIndex++] = Vector3(halfCosThetas[i], -0.5f, halfSinThetas[i]);
			uvs[uvIndex++] = Vector2(curU, 1.0f);

			curU -= stepU;
		}
		// Sides (excep the last quad)
		for (int i = 0; i < sides; ++i)
		{
			// Top tri
			indices[triIndex++] = firstSidePos + verticesPerCircle + i + 1; // bottom-right
			indices[triIndex++] = firstSidePos + i + 1; // top-right
			indices[triIndex++] = firstSidePos + i; // top-left

			// Bottom tri
			indices[triIndex++] = firstSidePos + verticesPerCircle + i; // bottom-left
			indices[triIndex++] = firstSidePos + verticesPerCircle + i + 1; // bottom-right
			indices[triIndex++] = firstSidePos + i; // top-left
		}

		// Combine vertex data
		std::vector<Vertex> vertices(vertexCount);
		for (int i = 0; i < vertexCount; ++i)
		{
			vertices[i] = Vertex(positions[i], Colour::White, uvs[i]);
		}

		// Vertex Buffer
		VertexBufferInstance* vertexBuffer = ResourceFactory::CreateVertexBufferInstance();
		vertexBuffer->SetVertices(vertices);

		// Index Buffer
		IndexBufferInstance* indexBuffer = ResourceFactory::CreateIndexBufferInstance();
		indexBuffer->SetIndices(indices);

		// Render Object
		RenderObject* renderObject = new RenderObject(name);
		renderObject->SetVertexBuffer(vertexBuffer);
		renderObject->SetIndexBuffer(indexBuffer);

		// Transform
		renderObject->Transform.SetPosition(pos);
		renderObject->Transform.SetRotation(rot);
		renderObject->Transform.SetScale(scale);

		return renderObject;
	}

	ENGINE_API RenderObject* Primitives::CreateQuad(Vector3 pos, Quaternion rot, Vector3 scale)
	{
		return CreateQuad("Quad", pos, rot, scale);
	}

	ENGINE_API RenderObject* Primitives::CreateQuad(std::string name, Vector3 pos, Quaternion rot, Vector3 scale)
	{
		// Vertex Buffer
		std::vector<Vertex> vertices
		{
			Vertex(Vector3(-0.5f, 0.5f, 0.0f), Colour::White, Vector2(0, 0)),
			Vertex(Vector3(0.5f, 0.5f, 0.0f), Colour::White, Vector2(1, 0)),
			Vertex(Vector3(-0.5f, -0.5f, 0.0f), Colour::White, Vector2(0, 1)),
			Vertex(Vector3(0.5f, -0.5f, 0.0f), Colour::White, Vector2(1, 1))
		};
		VertexBufferInstance* vertexBuffer = ResourceFactory::CreateVertexBufferInstance();
		vertexBuffer->SetVertices(vertices);

		// Index Buffer
		std::vector<int> indices { 0, 1, 2, 1, 3, 2 };
		IndexBufferInstance* indexBuffer = ResourceFactory::CreateIndexBufferInstance();
		indexBuffer->SetIndices(indices);

		// Render Object
		RenderObject* renderObject = new RenderObject(name);
		renderObject->SetVertexBuffer(vertexBuffer);
		renderObject->SetIndexBuffer(indexBuffer);

		// Transform
		renderObject->Transform.SetPosition(pos);
		renderObject->Transform.SetRotation(rot);
		renderObject->Transform.SetScale(scale);

		return renderObject;
	}
}
