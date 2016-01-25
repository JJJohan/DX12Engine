#pragma once

#include "BufferBucket.h"

namespace Engine
{
	class VertexBuffer : public BufferBucket
	{
	public:
		VertexBuffer();

	private:
		void Build() override;
		void Bind(ID3D12GraphicsCommandList* commandList) override;

		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

		friend class VertexBufferInstance;
	};
}

