#pragma once

#include "Factory.h"

namespace Engine
{
	class Material;

	class MaterialFactory : public Factory
	{
	public:
		static Material* CreateMaterial();
	};
}

