#pragma once

#include "Factory.h"

namespace Engine
{
	class RenderObject;

	class RenderObjectFactory : public Factory
	{
	public:
		static RenderObject* CreateRenderObject();
	};
}

