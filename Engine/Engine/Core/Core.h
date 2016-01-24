#pragma once

#include "../Rendering/Renderer.h"

namespace Engine
{
	struct ENGINE_LINK_DESC
	{
		std::function<void()> CreateMethod;
		std::function<void()> UpdateLoop;
		std::function<void()> DrawLoop;
		std::function<void()> DestroyMethod;
	};

	class Core
	{
	public:
		static ENGINE_API bool Update();
		static ENGINE_API std::string GetApplicationDirectory();
		static ENGINE_API bool Running();
		static ENGINE_API void Exit();
		static ENGINE_API void Destroy();
		static ENGINE_API void Initialise(int width, int height, bool windowed, ENGINE_LINK_DESC engineLink);

	private:
		static void Render();

		static bool _renderThreadAssigned;
		static bool _running;
		static std::string _appDirectory;
		static IRenderer* _renderer;

		static std::function<void()> _updateLoop;
		static std::function<void()> _destroyMethod;
	};
}

