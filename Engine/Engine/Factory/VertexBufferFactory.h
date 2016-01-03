#pragma once

#include "Factory.h"
#include "../Rendering/DX12/VertexBuffer.h"

namespace Engine
{
	class VertexBufferFactory : public Factory
	{
	public:
		template <typename T>
		static VertexBuffer<T>* CreateVertexBuffer()
		{
			VertexBuffer<T>* vertexBuffer = new VertexBuffer<T>();

			vertexBuffer->_pDevice = _pRenderer->_device.Get();

			return vertexBuffer;
		}
	};
}

