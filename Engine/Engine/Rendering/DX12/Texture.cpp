#include "Texture.h"
#include "HeapManager.h"
#include "../../External/src/FreeImage/FreeImage.h"

namespace Engine
{
	Texture::Texture(int width, int height)
		: _customDesc(false)
		, _pSrvHeap(nullptr)
		, _width(width)
		, _height(height)
		, _size(0)
		, _rootSlot(1)
	{
		_index = ResourceFactory::GetTextureSlot();
	}

	Texture::~Texture()
	{
		if (_index != -1)
		{
			ResourceFactory::FreeTextureSlot(_index);
		}
	}

	void Texture::SetRootSlot(int value)
	{
		_rootSlot = value;
	}

	bool Texture::Load(const std::string& filePath)
	{
		const char* filename = filePath.c_str();

		// Pointer to the image, once loaded
		FIBITMAP* dib = nullptr;

		// Check the file signature and deduce its format
		FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename, 0);

		// If still unknown, try to guess the file format from the file extension
		if (fif == FIF_UNKNOWN)
		{
			fif = FreeImage_GetFIFFromFilename(filename);
		}

		// If still unkown, return failure
		if (fif == FIF_UNKNOWN)
		{
			Logging::LogError("File specified is not a supported image file.");
			return false;
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
			return false;
		}

		// Convert to 32-bit.
		if (FreeImage_GetBPP(dib) != 32)
		{
			FIBITMAP* tmp = FreeImage_ConvertTo32Bits(dib);
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

		return true;
	}

	void Texture::Fill()
	{
		_size = _width * _height * 4;
		_fileBuffer = std::unique_ptr<BYTE>(new BYTE[_size]);
		BYTE* buffer = _fileBuffer.get();
		for (int i = 0; i < _size;)
		{
			buffer[i++] = 0;
			buffer[i++] = 0;
			buffer[i++] = 0;
			buffer[i++] = 255;
		}
	}

	void Texture::SetResourceDescription(const D3D12_RESOURCE_DESC& desc)
	{
		_textureDesc = desc;
		_customDesc = true;
	}

	void Texture::Apply()
	{
		HeapTask(std::bind(&Texture::Finalise, this));
	}

	void Texture::Finalise()
	{
		// Check we have loaded a texture, otherwise fill the texture with a black colour.
		if (_fileBuffer == nullptr)
		{
			Fill();
		}

		// Describe and create a Texture2D description.
		if (!_customDesc)
		{
			_textureDesc = {};
			_textureDesc.MipLevels = 1;
			_textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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
		HeapManager::Upload(this, _fileBuffer.get(), _width * 4, int(_heapSize));

		// Create descriptor handle if custom one is not provided.
		_heapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pSrvHeap->GetCPUDescriptorHandleForHeapStart(), _index, D3DUtils::GetSRVDescriptorSize());

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = _textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		_pDevice->CreateShaderResourceView(_pResource, &srvDesc, _heapHandle);
	}

	void Texture::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(_pSrvHeap->GetGPUDescriptorHandleForHeapStart(), _index, D3DUtils::GetSRVDescriptorSize());
		commandList->SetGraphicsRootDescriptorTable(_rootSlot, srvHandle);
	}
}

