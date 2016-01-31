#include "Material.h"
#include "Texture.h"
#include "../../Data/String.h"
#include "Engine/Rendering/DX12/Material.h"

namespace EngineNET
{
	Material::Material(Engine::Material* material)
	{
		_pMaterial = material;
	}

	Material::~Material()
	{
		this->!Material();
	}

	Material::!Material()
	{
		delete _pMaterial;
	}

	Material::operator Engine::Material*()
	{
		return _pMaterial;
	}

	void Material::LoadVertexShader(System::String^ shaderPath, System::String^ entryPoint, System::String^ shaderVersion)
	{
		_pMaterial->LoadVertexShader(String::Sys2Std(shaderPath), String::Sys2Std(entryPoint), String::Sys2Std(shaderVersion));
	}

	void Material::LoadPixelShader(System::String^ shaderPath, System::String^ entryPoint, System::String^ shaderVersion)
	{
		_pMaterial->LoadPixelShader(String::Sys2Std(shaderPath), String::Sys2Std(entryPoint), String::Sys2Std(shaderVersion));
	}

	void Material::Finalise(bool alpha)
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputDesc = 		
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		_pMaterial->Finalise(inputDesc, alpha);
	}

	void Material::SetTexture(Texture^ texture)
	{
		_pMaterial->SetTexture(texture);
	}

	Texture^ Material::GetTexture()
	{
		return gcnew Texture(_pMaterial->GetTexture());
	}
}