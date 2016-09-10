#pragma once

#include "HeapResource.h"

namespace Engine
{
	class RenderTexture : public HeapResource
	{
	public:
		ENGINE_API ~RenderTexture();

		void Apply();
		void SetDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE handle);

	private:
		RenderTexture(int width, int height);

		void Finalise();

		D3D12_RESOURCE_DESC _textureDesc;
		bool _customDesc;

		ID3D12DescriptorHeap* _pRtvHeap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE _heapHandle;

		// Image data
		int _width;
		int _height;
		size_t _size;

		friend class ResourceFactory;
	};
}

