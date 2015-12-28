#include "RenderObject.h"
#include "../../Utils/Logging.h"
#include "d3dx12.h"
#include <vector>
#include <D3Dcompiler.h>

namespace Engine
{
#ifdef _DEBUG
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	RenderObject::RenderObject()
		: _indexCount(0)
		, _vertexCount(0)
	{		
	}

	RenderObject::~RenderObject()
	{
	}

	void RenderObject::LoadVertexShader(std::string shaderPath, std::string entryPoint, std::string shaderVersion)
	{
		ComPtr<ID3DBlob> errorMessage;
		std::wstring widePath = std::wstring(shaderPath.begin(), shaderPath.end()).c_str();
		D3DCompileFromFile(widePath.c_str(), nullptr, nullptr, entryPoint.c_str(), shaderVersion.c_str(), compileFlags, 0, &_vertexShader, &errorMessage);
		if (errorMessage.Get() != nullptr)
		{
			Logging::LogError(static_cast<LPCTSTR>(errorMessage->GetBufferPointer()));
		}
		else if (_vertexShader.Get() == nullptr)
		{
			Logging::LogError("Failed to load vertex shader. No compilation error available.");
		}
	}

	void RenderObject::LoadPixelShader(std::string shaderPath, std::string entryPoint, std::string shaderVersion)
	{
		ComPtr<ID3DBlob> errorMessage;
		std::wstring widePath = std::wstring(shaderPath.begin(), shaderPath.end()).c_str();
		D3DCompileFromFile(widePath.c_str(), nullptr, nullptr, entryPoint.c_str(), shaderVersion.c_str(), compileFlags, 0, &_pixelShader, &errorMessage);
		if (errorMessage.Get() != nullptr)
		{
			Logging::LogError(static_cast<LPCTSTR>(errorMessage->GetBufferPointer()));
		}
		else if (_pixelShader.Get() == nullptr)
		{
			Logging::LogError("Failed to load pixel shader. No compilation error available.");
		}
	}

	ID3DBlob* RenderObject::GetVertexShader() const
	{
		return _vertexShader.Get();
	}

	ID3DBlob* RenderObject::GetPixelShader() const
	{
		return _pixelShader.Get();
	}

	void RenderObject::Draw(ID3D12GraphicsCommandList* commandList) const
	{
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
		if (_indexBuffer == nullptr)
		{
			commandList->DrawInstanced(UINT(_vertexCount), 1, 0, 0);
		}
		else
		{
			commandList->DrawIndexedInstanced(UINT(_indexCount), 0, 0, 0, 0);
		}
	}

	void RenderObject::SetVertices(ID3D12Device* device, std::vector<Vertex> vertices)
	{
		_vertices = vertices;
		_vertexCount = _vertices.size();
		const UINT vertexBufferSize = UINT(sizeof(Vertex) * _vertexCount);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		LOGFAILEDCOM(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&_vertexBuffer)));

		UINT8* pVertexDataBegin;
		LOGFAILEDCOM(
			_vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, &_vertices[0], vertexBufferSize);
		_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
		_vertexBufferView.StrideInBytes = sizeof(Vertex);
		_vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	std::vector<Vertex> RenderObject::GetVertices() const
	{
		return _vertices;
	}

	void RenderObject::SetIndices(ID3D12Device* device, std::vector<int> indices)
	{
		_indices = indices;
		_indexCount = _indices.size();
		const UINT indexBufferSize = UINT(sizeof(int) * _indexCount);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		LOGFAILEDCOM(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&_indexBuffer)));

		UINT8* pIndexDataBegin;
		LOGFAILEDCOM(
			_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pIndexDataBegin)));
		memcpy(pIndexDataBegin, &_vertices[0], indexBufferSize);
		_indexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
		_indexBufferView.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_indexBufferView.SizeInBytes = indexBufferSize;
	}

	std::vector<int> RenderObject::GetIndices() const
	{
		return _indices;
	}
}
