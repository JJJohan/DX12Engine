#include "Material.h"
#include "Texture.h"
#include "GBuffer.h"

using namespace Microsoft::WRL;

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
	std::vector<D3D12_INPUT_ELEMENT_DESC> Material::Default_Input_Layout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	ID3D12PipelineState* Material::GetPSO(const void* vertexByteCode, const void* pixelByteCode, bool alpha)
	{
		// Check if the PSO already exists and use this.
		for (auto it = _psoCache.begin(); it != _psoCache.end(); ++it)
		{
			PSOCacheItem& cacheItem = *it;
			if (cacheItem.VertexByteCode == vertexByteCode && cacheItem.PixelByteCode == pixelByteCode && cacheItem.Alpha == alpha)
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
		, _pTexture(nullptr)
		, _pDevice(nullptr)
		, _pRootSignature(nullptr) { }

	void Material::LoadVertexShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderVersion)
	{
		ID3DBlob* errorMessage = nullptr;
		std::wstring widePath = std::wstring(shaderPath.begin(), shaderPath.end());
		D3DCompileFromFile(widePath.c_str(), nullptr, nullptr, entryPoint.c_str(), shaderVersion.c_str(), compileFlags, 0, &_pVertexShader, &errorMessage);
		
		if (!D3DUtils::Succeeded(errorMessage))
		{
			return;
		}

		if (_pVertexShader == nullptr)
		{
			Logging::LogError("Failed to load vertex shader. No compilation error available.");
		}
	}

	void Material::LoadPixelShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderVersion)
	{
		ID3DBlob* errorMessage = nullptr;
		std::wstring widePath = std::wstring(shaderPath.begin(), shaderPath.end());
		D3DCompileFromFile(widePath.c_str(), nullptr, nullptr, entryPoint.c_str(), shaderVersion.c_str(), compileFlags, 0, &_pPixelShader, &errorMessage);
		
		if (!D3DUtils::Succeeded(errorMessage))
		{
			return;
		}

		if (_pPixelShader == nullptr)
		{
			Logging::LogError("Failed to load pixel shader. No compilation error available.");
		}
	}

	void Material::Finalise(std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout, bool alpha, bool deferred)
	{
		_pPipelineState = GetPSO(_pVertexShader->GetBufferPointer(), _pPixelShader->GetBufferPointer(), alpha);

		// Create the pipeline state, which includes compiling and loading shaders.
		if (_pPipelineState == nullptr)
		{
			CD3DX12_BLEND_DESC blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			if (alpha)
			{
				blendState.RenderTarget[0].BlendEnable = true;
				blendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				blendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
				blendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
				blendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			}

			// Describe and create the graphics pipeline state object (PSO).
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = {&inputLayout[0], UINT(inputLayout.size())};
			psoDesc.pRootSignature = _pRootSignature;
			psoDesc.VS = {reinterpret_cast<UINT8*>(_pVertexShader->GetBufferPointer()), _pVertexShader->GetBufferSize()};
			psoDesc.PS = {reinterpret_cast<UINT8*>(_pPixelShader->GetBufferPointer()), _pPixelShader->GetBufferSize()};
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.BlendState = blendState;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.SampleDesc.Count = 1;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			if (deferred)
			{
				psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
				psoDesc.DepthStencilState.DepthEnable = TRUE;
				psoDesc.DepthStencilState.StencilEnable = FALSE;
				psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

				psoDesc.NumRenderTargets = GBuffer::GBUFFER_NUM_TEXTURES;			
				for (size_t i = 0; i < GBuffer::GBUFFER_NUM_TEXTURES; ++i)
				{
					psoDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
				}
			}
			else
			{
				psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
				psoDesc.DepthStencilState.DepthEnable = FALSE;
				psoDesc.DepthStencilState.StencilEnable = FALSE;
				psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				
				psoDesc.NumRenderTargets = 1;
				psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			}

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

	void Material::SetTexture(Texture* texture)
	{
		_pTexture = texture;
	}

	Texture* Material::GetTexture() const
	{
		return _pTexture;
	}

	void Material::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		if (_pPipelineState != _pLastPipelineState)
		{
			commandList->SetPipelineState(_pPipelineState);
			_pLastPipelineState = _pPipelineState;
		}

		if (_pTexture != nullptr)
		{
			_pTexture->Bind(commandList);
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

