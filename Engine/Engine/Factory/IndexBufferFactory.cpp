#include "IndexBufferFactory.h"
#include "../Rendering/DX12/IndexBuffer.h"

namespace Engine
{
	IndexBuffer* IndexBufferFactory::CreateIndexBuffer()
	{
		IndexBuffer* indexBuffer = new IndexBuffer();

		indexBuffer->_pDevice = _pRenderer->_device.Get();

		return indexBuffer;
	}
}

