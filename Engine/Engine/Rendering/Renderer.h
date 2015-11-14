#pragma once

#include <memory>
#include "IRenderer.h"

namespace Engine
{
	enum RenderAPI
	{
		Direct3D12
	};

	class Renderer
	{
	public:
		~Renderer();

		static IRenderer* CreateRenderer(RenderAPI renderAPI);
		static void DestroyRenderer();
		static bool Render();
		static bool Update();

	private:
		Renderer();
	};

	
}
