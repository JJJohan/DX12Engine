#include "Texture.h"
#include <fstream>
#include "../../Utils/Logging.h"
#include <sstream>
#include <memory>
#include "d3dx12.h"
#include "../../Factory/Factory.h"
#include "CommandQueue.h"
#include "HeapManager.h"
#include "../../External/src/FreeImage/FreeImage.h"

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

	void Texture::Load(const std::string& filePath)
	{
		const char* filename = filePath.c_str();

		// Image format
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

		// Pointer to the image, once loaded
		FIBITMAP *dib = nullptr;

		// Check the file signature and deduce its format
		fif = FreeImage_GetFileType(filename, 0);

		// If still unknown, try to guess the file format from the file extension
		if (fif == FIF_UNKNOWN)
		{
			fif = FreeImage_GetFIFFromFilename(filename);
		}

		// If still unkown, return failure
		if (fif == FIF_UNKNOWN)
		{
			Logging::LogError("File specified is not a supported image file.");
			return;
		}

		// Check that the plugin has reading capabilities and load the file
		if (FreeImage_FIFSupportsReading(fif))
		{
			dib = FreeImage_Load(fif, filename);
		}
		
		// If the image failed to load, return failure
		if (!dib)
		{
			Logging::LogError("Failed to load image file.");
			return;
		}

		// Convert to 32-bit.
		if (FreeImage_GetBPP(dib) != 32)
		{
			FIBITMAP *tmp = FreeImage_ConvertTo32Bits(dib);
			FreeImage_Unload(dib);
			dib = tmp;
		}

		// Correct the image.
		FreeImage_FlipVertical(dib);

		// Retrieve the image data
		BYTE* bytes = FreeImage_GetBits(dib);
		_size = FreeImage_GetDIBSize(dib);

		// Get the image width and height
		_width = FreeImage_GetWidth(dib);
		_height = FreeImage_GetHeight(dib);

		_fileBuffer = std::unique_ptr<BYTE>(new BYTE[_size]);
		memcpy(_fileBuffer.get(), bytes, _size);

		// Free FreeImage's copy of the data
		FreeImage_Unload(dib);

		HeapTask(std::bind(&Texture::Finalise, this));
	}

	void Texture::Finalise()
	{
		ID3D12GraphicsCommandList* commandList = static_cast<ID3D12GraphicsCommandList*>(Factory::GetCommandList());

		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.Width = _width;
		textureDesc.Height = _height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		// Allocate espace and upload to the heap.
		PrepareHeapResource(_size, textureDesc);
		_heapSize = GetRequiredIntermediateSize(_pResource, 0, 1);
		HeapManager::Upload(this, _fileBuffer.get(), _width * 4, int(_heapSize), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// Describe and create a SRV for the texture.
		_descSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(_pSrvHeap->GetCPUDescriptorHandleForHeapStart(), _index, _descSize);
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

