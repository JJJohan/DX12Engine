#include "Material.h"
#include "../../Utils/Logging.h"
#include <D3Dcompiler.h>
#include "d3dx12.h"

#ifdef _DEBUG
// Enable better shader debugging with the graphics debugging tools.
UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
UINT compileFlags = 0;
#endif

namespace Engine
{
	ID3D12PipelineState* Material::_pLastPipelineState = nullptr;
	std::vector<PSOCacheItem> Material::_psoCache;

	ID3D12PipelineState* Material::GetPSO(const void* vertexByteCode, const void* pixelByteCode)
	{
		// Check if the PSO already exists and use this.
		for (auto it = _psoCache.begin(); it != _psoCache.end(); ++it)
		{
			PSOCacheItem& cacheItem = *it;
			if (cacheItem.VertexByteCode == vertexByteCode && cacheItem.PixelByteCode == pixelByteCode)
			{
				return cacheItem.PipelineState;
			}
		}

		return nullptr;
	}

	void Material::ReleasePSOCache()
	{
		for (auto it = _psoCache.begin(); it != _psoCache.end(); ++it)
		{
			PSOCacheItem& cacheItem = *it;
			cacheItem.PipelineState->Release();
		}
		_psoCache.clear();
	}

	Material::Material()
		: _pPipelineState(nullptr)
		, _pVertexShader(nullptr)
		, _pPixelShader(nullptr)
		, _pDevice(nullptr)
		, _pRootSignature(nullptr)
	{
		
	}

	void Material::LoadVertexShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderVersion)
	{
		ComPtr<ID3DBlob> errorMessage;
		std::wstring widePath = std::wstring(shaderPath.begin(), shaderPath.end());
		D3DCompileFromFile(widePath.c_str(), nullptr, nullptr, entryPoint.c_str(), shaderVersion.c_str(), compileFlags, 0, &_pVertexShader, &errorMessage);
		if (errorMessage.Get() != nullptr)
		{
			Logging::LogError(static_cast<LPCTSTR>(errorMessage->GetBufferPointer()));
		}
		else if (_pVertexShader == nullptr)
		{
			Logging::LogError("Failed to load vertex shader. No compilation error available.");
		}
	}

	void Material::LoadPixelShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderVersion)
	{
		ComPtr<ID3DBlob> errorMessage;
		std::wstring widePath = std::wstring(shaderPath.begin(), shaderPath.end());
		D3DCompileFromFile(widePath.c_str(), nullptr, nullptr, entryPoint.c_str(), shaderVersion.c_str(), compileFlags, 0, &_pPixelShader, &errorMessage);
		if (errorMessage.Get() != nullptr)
		{
			Logging::LogError(static_cast<LPCTSTR>(errorMessage->GetBufferPointer()));
		}
		else if (_pPixelShader == nullptr)
		{
			Logging::LogError("Failed to load pixel shader. No compilation error available.");
		}
	}

	void Material::Finalise(std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout)
	{
		_pPipelineState = GetPSO(_pVertexShader->GetBufferPointer(), _pPixelShader->GetBufferPointer());

		// Create the pipeline state, which includes compiling and loading shaders.
		if (_pPipelineState == nullptr)
		{
			// Describe and create the graphics pipeline state object (PSO).
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = {&inputLayout[0], UINT(inputLayout.size())};
			psoDesc.pRootSignature = _pRootSignature;
			psoDesc.VS = {reinterpret_cast<UINT8*>(_pVertexShader->GetBufferPointer()), _pVertexShader->GetBufferSize()};
			psoDesc.PS = {reinterpret_cast<UINT8*>(_pPixelShader->GetBufferPointer()), _pPixelShader->GetBufferSize()};
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			psoDesc.SampleMask = UINT_MAX ;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;

			LOGFAILEDCOM(_pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pPipelineState)));

			PSOCacheItem cacheItem;
			cacheItem.PipelineState = _pPipelineState;
			cacheItem.VertexShader = _pVertexShader;
			cacheItem.VertexByteCode = _pVertexShader->GetBufferPointer();
			cacheItem.PixelShader = _pPixelShader;
			cacheItem.PixelByteCode = _pPixelShader->GetBufferPointer();
			_psoCache.push_back(cacheItem);
		}
	}

	void Material::VerifyPSO(ID3D12GraphicsCommandList* commandList) const
	{
		if (_pPipelineState != _pLastPipelineState)
		{
			commandList->SetPipelineState(_pPipelineState);
			_pLastPipelineState = _pPipelineState;
		}
	}

	void Material::ClearPSOHistory()
	{
		_pLastPipelineState = nullptr;
	}

	ID3DBlob* Material::GetVertexShader() const
	{
		return _pVertexShader;
	}

	ID3DBlob* Material::GetPixelShader() const
	{
		return _pPixelShader;
	}

	ID3D12PipelineState* Material::GetPipelineState() const
	{
		return _pPipelineState;
	}
}
