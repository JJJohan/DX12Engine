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

		bool Load(const std::string& filePath);
		void Bind(ID3D12GraphicsCommandList* commandList) const;

	private:
		Texture();

		void Finalise();

		std::unique_ptr<BYTE> _fileBuffer;
		ID3D12DescriptorHeap* _pSrvHeap;
		int _index;

		// Image data
		int _width;
		int _height;
		size_t _size;

		static UINT _descSize;

		friend class ResourceFactory;
	};
}

