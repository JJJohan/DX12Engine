#pragma once

#include <string>
#include <d3d12.h>
#include <wrl/client.h>
#include <memory>
#include "HeapResource.h"

using namespace Microsoft::WRL;

namespace Engine
{
	class Texture : HeapResource
	{
	public:
		~Texture();

		void LoadFromDDS(const std::string& filePath);

	private:
		Texture();

		void Load();

		std::unique_ptr<char[]> _fileBuffer;
		int _width;
		int _height;

		ID3D12DescriptorHeap* _pSrvHeap;

		friend class TextureFactory;
	};
}

