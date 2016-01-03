#include "Texture.h"
#include <fstream>
#include "../../Utils/Logging.h"
#include <sstream>
#include <memory>
#include "d3dx12.h"
#include "DX12Renderer.h"
#include "../../Factory/Factory.h"
#include "CommandQueue.h"
#include "HeapManager.h"

namespace Engine
{
	Texture::Texture()
		: _width(0)
		, _height(0)
	{
	}

	Texture::~Texture()
	{
	}

	void Texture::LoadFromDDS(const std::string& filePath)
	{
		// Open the file
		std::fstream file;
		file.open(filePath, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
		if (!file.is_open())
		{
			std::stringstream ss;
			ss << "Texture::LoadDDS() : Could not open file at '" << filePath << "'.\n";
			Logging::LogError(ss.str());
			return;
		}

		// Read the file
		size_t size = file.tellg();
		_fileBuffer = std::unique_ptr<char[]>(new char[size]);
		file.seekg(std::ios_base::beg);
		file.read(_fileBuffer.get(), size);

		file.close();

		HeapTask(std::bind(&Texture::Load, this));
	}

	void Texture::Load()
	{
		ID3D12GraphicsCommandList* commandList = static_cast<ID3D12GraphicsCommandList*>(Factory::GetCommandList());

		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = _width;
		textureDesc.Height = _height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		size_t textureSize = _width * _height * 4;
		PrepareHeapResource(textureSize, textureDesc);

		_heapSize = GetRequiredIntermediateSize(_pResource, 0, 1);
		HeapManager::Upload(this, _fileBuffer.get(), _width * 4, _width * 4 * _height, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		_pDevice->CreateShaderResourceView(_pResource, &srvDesc, _pSrvHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

