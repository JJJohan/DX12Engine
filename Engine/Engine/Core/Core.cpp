#include "Core.h"

namespace Engine
{
	bool Core::Running = true;

	bool Core::Update()
	{
		Time::Update();

		if (Renderer::Update() == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	bool Core::Render()
	{
		return Renderer::Render();
	}

	void Core::Exit()
	{
		Running = false;
	}

	void Core::Destroy()
	{
		Renderer::DestroyRenderer();
	}
}