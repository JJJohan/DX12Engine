#include "Core.h"
#include "../Utils/Logging.h"
#include "../Input/Input.h"
#include "Time.h"

namespace Engine
{
	std::string Core::_appDirectory = std::string();
	bool Core::_running = true;
	std::thread Core::_renderThread = std::thread();
	bool Core::_renderThreadAssigned = false;
	IRenderer* Core::_renderer = nullptr;
	std::function<void()> Core::_updateLoop;
	std::function<void()> Core::_destroyMethod;

	bool Core::Update()
	{
		Time::Update();

		if (_renderer == nullptr)
		{
			Logging::LogError("Tried to update core without a renderer!");
			return EXIT_FAILURE;
		}

		if (_renderer->WindowRender() == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		Input::Update();

		// Call the main update loop.
		_updateLoop();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		return EXIT_SUCCESS;
	}

	void Core::Render()
	{
		while (Running())
		{
			if (_renderer->RenderFinished())
			{
				if (_renderer->Render())
				{
					_running = false;
				}
			}
		}
	}

	void Core::Initialise(int width, int height, bool windowed, ENGINE_LINK_DESC engineLink)
	{
		Time::Initialise();

		_updateLoop = engineLink.UpdateLoop;
		_destroyMethod = engineLink.DestroyMethod;

		Input::RegisterKey(VK_ESCAPE, KeyDown, []
		                            {
			                            Exit();
		                            }, "__exit");

		_renderer = Renderer::GetRenderer();
		if (_renderer == nullptr)
		{
			Logging::LogError("Tried to begin rendering without a renderer!");
			return;
		}

		_renderer->AssignCreateMethod(engineLink.CreateMethod);
		_renderer->AssignDrawLoop(engineLink.DrawLoop);

		if (_renderer->InitWindow(width, height, windowed) != EXIT_FAILURE)
		{
			_renderer->SetClearColour(Colour(0.1f, 0.1f, 0.1f));
			_renderer->SetVsync(false);
		}

		if (!_renderThreadAssigned)
		{
			_renderThread = std::thread(Render);
			_renderThreadAssigned = true;
		}
	}

	void Core::Exit()
	{
		_running = false;
	}

	bool Core::Running()
	{
		return _running;
	}

	void Core::Destroy()
	{
		Exit();

		while (_renderThreadAssigned)
		{
			if (_renderThread.joinable())
			{
				_renderThread.join();
				_renderThreadAssigned = false;
			}
		}

		_destroyMethod();

		Renderer::DestroyRenderer();
	}

	std::string Core::GetApplicationDirectory()
	{
		if (_appDirectory.empty())
		{
			char dir[512];
			GetModuleFileNameA(nullptr, dir, sizeof(dir));
			_appDirectory = std::string(dir);

			size_t seperatorIndex = _appDirectory.find_last_of('\\');
			if (seperatorIndex != -1)
			{
				_appDirectory = _appDirectory.substr(0, seperatorIndex + 1);
			}
		}

		return _appDirectory;
	}
}

