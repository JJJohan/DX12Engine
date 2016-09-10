#include "RenderTexture.h"
#include "HeapManager.h"

namespace Engine
{
	RenderTexture::RenderTexture(int width, int height)
		: _customDesc(false)
		, _pRtvHeap(nullptr)
		, _width(width)
		, _height(height)
		, _size(0)
	{

	}

	RenderTexture::~RenderTexture()
	{

	}

	void RenderTexture::Apply()
	{
		HeapTask(std::bind(&RenderTexture::Finalise, this));
	}

	void RenderTexture::SetDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE handle)
	{
		_heapHandle = handle;
	}

	void RenderTexture::Finalise()
	{
		// Describe and create a Texture2D description.
		if (!_customDesc)
		{
			_textureDesc = {};
			_textureDesc.MipLevels = 1;
			_textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			_textureDesc.Width = _width;
			_textureDesc.Height = _height;
			_textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			_textureDesc.DepthOrArraySize = 1;
			_textureDesc.SampleDesc.Count = 1;
			_textureDesc.SampleDesc.Quality = 0;
			_textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		}

		// Allocate space and upload to the heap.
		_heapSize = _width * _height * 4;
		PrepareHeapResource(_textureDesc);
		char* clear = new char[_heapSize];
		ZeroMemory(clear, _heapSize);
		HeapManager::Upload(this, clear, _width * 4, int(_heapSize), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		delete[] clear;

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = _textureDesc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT ? DXGI_FORMAT_B8G8R8A8_UNORM : _textureDesc.Format;;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		_pDevice->CreateShaderResourceView(_pResource, &srvDesc, _heapHandle);
	}
}

