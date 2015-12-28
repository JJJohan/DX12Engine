#include "Core.h"

namespace Engine
{	
	std::string Core::_appDirectory = std::string();
	bool Core::_running = true;
	std::thread Core::_renderThread = std::thread();
	bool Core::_renderThreadAssigned = false;
	IRenderer* Core::_renderer = nullptr;

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

		return EXIT_SUCCESS;
	}

	void Core::Render()
	{
		while (Running())
		{
			if (_renderer->RenderFinished())
			{
				Time::UpdateGPU();

				if (_renderer->Render())
				{
					_running = false;
				}
			}
		}
	}

	void Core::BeginRendering()
	{
		_renderer = Renderer::GetRenderer();
		if (_renderer == nullptr)
		{
			Logging::LogError("Tried to begin rendering without a renderer!");
			return;
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

		Renderer::DestroyRenderer();
	}

	std::string Core::GetApplicationDirectory()
	{
		if (_appDirectory.empty())
		{
			char dir[512];
			GetModuleFileName(nullptr, dir, sizeof(dir));
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