#pragma once
#include <string>
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>

using namespace Microsoft::WRL;

namespace Engine
{
	struct PSOCacheItem
	{
		ID3D12PipelineState* PipelineState;
		ID3DBlob* VertexShader;
		ID3DBlob* PixelShader;
		const void* VertexByteCode;
		const void* PixelByteCode;
	};

	class Texture;

	class Material
	{
	public:
		void LoadVertexShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderVersion);
		void LoadPixelShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderVersion);
		ID3DBlob* GetVertexShader() const;
		ID3DBlob* GetPixelShader() const;

		ID3D12PipelineState* GetPipelineState() const;

		void Finalise(std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout);

		void Bind(ID3D12GraphicsCommandList* commandList) const;
		static void ClearPSOHistory();
		static void ReleasePSOCache();

		void SetTexture(Texture* texture);
		Texture* GetTexture() const;

	private:
		Material();

		static ID3D12PipelineState* _pLastPipelineState;
		static std::vector<PSOCacheItem> _psoCache;

		static ID3D12PipelineState* GetPSO(const void* vertexByteCode, const void* pixelByteCode);

		ID3D12PipelineState* _pPipelineState;
		ID3DBlob* _pVertexShader;
		ID3DBlob* _pPixelShader;
		Texture* _pTexture;

		ID3D12Device* _pDevice;
		ID3D12RootSignature* _pRootSignature;

		friend class ResourceFactory;
	};
}

