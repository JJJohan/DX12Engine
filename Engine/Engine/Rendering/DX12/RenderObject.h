#pragma once

#include "d3d12.h"
#include <wrl/client.h>
#include <DirectXMath.h>
#include <vector>

using namespace Microsoft::WRL;

namespace Engine
{
	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Colour;
		DirectX::XMFLOAT2 UV;
	};

	class RenderObject
	{
	public:
		RenderObject();
		~RenderObject();

		void SetVertices(ID3D12Device* device, std::vector<Vertex> vertices);
		std::vector<Vertex> GetVertices() const;

		void SetIndices(ID3D12Device* device, std::vector<int> indices);
		std::vector<int> GetIndices() const;

		void LoadVertexShader(std::string shaderPath, std::string entryPoint, std::string shaderVersion);
		void LoadPixelShader(std::string shaderPath, std::string entryPoint, std::string shaderVersion);
		ID3DBlob* GetVertexShader() const;
		ID3DBlob* GetPixelShader() const;

		void Draw(ID3D12GraphicsCommandList* commandList) const;

	private:
		size_t _indexCount;
		std::vector<int> _indices;
		ComPtr<ID3D12Resource> _indexBuffer;
		D3D12_INDEX_BUFFER_VIEW _indexBufferView;

		size_t _vertexCount;
		std::vector<Vertex> _vertices;
		ComPtr<ID3D12Resource> _vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

		ComPtr<ID3DBlob> _vertexShader;
		ComPtr<ID3DBlob> _pixelShader;
		DirectX::XMMATRIX _worldMatrix;
	};
}
