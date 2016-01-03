#pragma once

#include "../Rendering/Renderer.h"
#include "../Input/Input.h"
#include "../Data/Colour.h"
#include "../Data/Quaternion.h"
#include "../Data/Vector2.h"
#include "../Data/Vector3.h"
#include "../Core/Time.h"
#include "../Utils/Console.h"
#include "../Utils/Logging.h"
#include "../Utils/SystemInfo.h"
#include <thread>

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
		static bool Update();
		static void Render();

		static std::string GetApplicationDirectory();
		static bool Running();
		static void Exit();
		static void Destroy();
		static void Initialise(int width, int height, bool windowed, ENGINE_LINK_DESC engineLink);

	private:
		static std::thread _renderThread;
		static bool _renderThreadAssigned;
		static bool _running;
		static std::string _appDirectory;
		static IRenderer* _renderer;

		static std::function<void()> _updateLoop;
		static std::function<void()> _destroyMethod;
	};
}

