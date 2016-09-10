#pragma once

namespace Engine
{
	struct PSOCacheItem
	{
		ID3D12PipelineState* PipelineState;
		ID3DBlob* VertexShader;
		ID3DBlob* PixelShader;
		const void* VertexByteCode;
		const void* PixelByteCode;
		bool Alpha;
	};

	enum DepthTest
	{
		ALWAYS,
		LESS_EQUAL,
		GREATER_EQUAL,
		EQUAL,
		NOT_EQUAL,
		LESS,
		GREATER,
		NEVER
	};

	class Texture;

	class Material
	{
	public:
		ENGINE_API void LoadVertexShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderVersion);
		ENGINE_API void LoadPixelShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderVersion);
		ID3DBlob* GetVertexShader() const;
		ID3DBlob* GetPixelShader() const;

		ID3D12PipelineState* GetPipelineState() const;

		ENGINE_API void Finalise(std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout, bool alpha = false, bool deferred = true);

		void Bind(ID3D12GraphicsCommandList* commandList) const;
		static void ClearPSOHistory();
		static void ReleasePSOCache();

		ENGINE_API void SetTexture(Texture* texture);
		ENGINE_API Texture* GetTexture() const;
		ENGINE_API void SetDepthMode(bool depthWrite, DepthTest depthTest);

		static ENGINE_API std::vector<D3D12_INPUT_ELEMENT_DESC> Default_Input_Layout;
		static ENGINE_API std::vector<D3D12_INPUT_ELEMENT_DESC> PBR_Input_Layout;

	private:
		Material();

		static ID3D12PipelineState* _pLastPipelineState;
		static std::vector<PSOCacheItem> _psoCache;

		static ID3D12PipelineState* GetPSO(const void* vertexByteCode, const void* pixelByteCode, bool alpha);

		ID3D12PipelineState* _pPipelineState;
		ID3DBlob* _pVertexShader;
		ID3DBlob* _pPixelShader;
		Texture* _pTexture;
		D3D12_COMPARISON_FUNC _depthCheck;
		D3D12_DEPTH_WRITE_MASK _depthWrite;

		ID3D12Device* _pDevice;
		ID3D12RootSignature* _pRootSignature;

		friend class ResourceFactory;
	};
}

