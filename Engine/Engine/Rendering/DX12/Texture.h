#pragma once

#include "HeapResource.h"

namespace Engine
{
	class Texture : HeapResource
	{
	public:
		ENGINE_API ~Texture();

		ENGINE_API bool Load(const std::string& filePath);
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

