#pragma once

#include "BufferBucket.h"
#include "BufferInstance.h"

namespace Engine
{
	class IndexBufferInstance;

	class IndexBuffer : public BufferBucket
	{
	public:
		IndexBuffer();

	private:
		void Build() override;
		void Bind(ID3D12GraphicsCommandList* commandList) override;

		D3D12_INDEX_BUFFER_VIEW _indexBufferView;

		friend class IndexBufferInstance;
	};

	class IndexBufferInstance : public BufferInstance
	{
	public:
		ENGINE_API void SetIndices(std::vector<int> indices);
		ENGINE_API std::vector<int> GetIndices() const;

		size_t Count() const;
		size_t GetSize() const override;

	private:
		std::vector<int> _indices;

		friend class IndexBuffer;
	};
}

