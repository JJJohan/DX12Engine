#pragma once

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

		static ENGINE_API IRenderer* CreateRenderer(RenderAPI renderAPI);
		static void DestroyRenderer();
		static bool Render();
		static bool Update();
		static HWND GetWindowHandle();
		static ENGINE_API IRenderer* GetRenderer();
		static ENGINE_API void ProcessWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		Renderer();
	};
}

