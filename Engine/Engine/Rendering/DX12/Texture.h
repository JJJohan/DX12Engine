#pragma once

#include <string>
#include <d3d12.h>
#include <wrl/client.h>
#include <memory>
#include "HeapResource.h"
#include <unordered_set>

using namespace Microsoft::WRL;

namespace Engine
{
	class Texture : HeapResource
	{
	public:
		~Texture();

		void LoadFromDDS(const std::string& filePath);
		void Bind(ID3D12GraphicsCommandList* commandList) const;

		const static int TextureLimit = 32;

	private:
		Texture();

		void Load();

		std::unique_ptr<char[]> _fileBuffer;
		ID3D12DescriptorHeap* _pSrvHeap;
		int _width;
		int _height;
		int _index;

		static int GetFreeHeapIndex();
		static UINT _descSize;
		static std::vector<bool> _heapSlotsInUse;

		friend class TextureFactory;
	};
}

