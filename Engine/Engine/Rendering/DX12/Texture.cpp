#include "Texture.h"
#include <fstream>
#include "../../Utils/Logging.h"
#include <sstream>
#include <memory>
#include "d3dx12.h"
#include "../../Factory/Factory.h"
#include "CommandQueue.h"
#include "HeapManager.h"

namespace Engine
{
	std::vector<bool> Texture::_heapSlotsInUse = std::vector<bool>(64);
	UINT Texture::_descSize = 0;

	Texture::Texture()
		: _pSrvHeap(nullptr)
		, _width(0)
		, _height(0)
	{
		_index = GetFreeHeapIndex();
		if (_index == -1)
		{
			Logging::LogError("Ran out of SRV slots. Consider increasing the TextureLimit constant.");
		}
	}

	Texture::~Texture()
	{
		if (_index != -1)
		{
			_heapSlotsInUse[_index] = false;
		}
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
		HeapManager::Upload(this, _fileBuffer.get(), _width * 4, int(_heapSize), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// Describe and create a SRV for the texture.
		_descSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(_pSrvHeap->GetCPUDescriptorHandleForHeapStart(), _index, _descSize);
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		_pDevice->CreateShaderResourceView(_pResource, &srvDesc, srvHandle);
	}

	void Texture::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(_pSrvHeap->GetGPUDescriptorHandleForHeapStart(), _index, _descSize);
		commandList->SetGraphicsRootDescriptorTable(1, srvHandle);
	}

	int Texture::GetFreeHeapIndex()
	{
		if (_heapSlotsInUse.empty())
		{
			for (int i = 0; i < TextureLimit; ++i)
			{
				_heapSlotsInUse.push_back(false);
			}
		}

		for (int i = 0; i < TextureLimit; ++i)
		{
			if (_heapSlotsInUse[i] == false)
			{
				_heapSlotsInUse[i] = true;
				return i;
			}
		}

		return -1;
	}
}

