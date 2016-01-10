#pragma once

#include <vector>
#include "d3dx12.h"
#include "HeapResource.h"

namespace Engine
{
	class IndexBuffer : HeapResource
	{
	public:
		void Bind(ID3D12GraphicsCommandList* commandList) const;
		int Count() const;

		void SetIndices(std::vector<int> indices);
		std::vector<int> GetIndices() const;

	private:
		IndexBuffer();
		void CreateIndexBuffer();

		std::vector<int> _indices;
		size_t _indexCount;
		D3D12_INDEX_BUFFER_VIEW _indexBufferView;

		friend class ResourceFactory;
	};
}

