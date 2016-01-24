#pragma once

#include "BufferBucket.h"

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
}

