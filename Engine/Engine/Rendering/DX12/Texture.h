#pragma once

#include "HeapResource.h"

namespace Engine
{
	class Texture : public HeapResource
	{
	public:
		ENGINE_API ~Texture();

		ENGINE_API bool Load(const std::string& filePath);
		void Bind(ID3D12GraphicsCommandList* commandList) const;
		void SetResourceDescription(const D3D12_RESOURCE_DESC& desc);
		void Apply();
		void SetDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE handle);

	private:
		Texture(int width, int height);

		void Finalise();
		void Fill();

		std::unique_ptr<BYTE> _fileBuffer;
		D3D12_RESOURCE_DESC _textureDesc;
		bool _customDesc;
		int _index;

		ID3D12DescriptorHeap* _pSrvHeap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE _heapHandle;

		// Image data
		int _width;
		int _height;
		size_t _size;

		friend class ResourceFactory;
	};
}

