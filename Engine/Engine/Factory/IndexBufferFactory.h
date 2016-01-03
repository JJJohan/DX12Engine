#pragma once

#include "Factory.h"

namespace Engine
{
	class IndexBuffer;

	class IndexBufferFactory : public Factory
	{
	public:
		static IndexBuffer* CreateIndexBuffer();
	};
}

