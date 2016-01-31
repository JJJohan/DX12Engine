#pragma once

#include "../../stdafx.h"

namespace Engine
{
	class Material;
}

namespace EngineNET
{
	ref class Texture;

	public ref class Material
	{
	public:
		Material(Engine::Material* material);
		~Material();
		!Material();
		operator Engine::Material*();

		void LoadVertexShader(System::String^ shaderPath, System::String^entryPoint, System::String^ shaderVersion);
		void LoadPixelShader(System::String^ shaderPath, System::String^ entryPoint, System::String^ shaderVersion);

		void Finalise(bool alpha);

		void SetTexture(Texture^ texture);
		Texture^ GetTexture();

	private:
		Engine::Material* _pMaterial;
	};
}