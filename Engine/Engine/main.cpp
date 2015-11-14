#include <iostream>
#include <thread>
#include <sstream>
#include "Core/Core.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
#ifdef _DEBUG
	Engine::Console::InitConsole();
#endif

	Engine::SystemInfo::PrintSystemInfo();
	Engine::Logging::Log("Starting engine...\n");

	Engine::IRenderer* renderer = Engine::Renderer::CreateRenderer(Engine::RenderAPI::Direct3D12);
	bool windowInit = false;
	if (renderer->InitWindow(1024, 768, true) != EXIT_FAILURE)
	{
		renderer->SetClearColour(Engine::Colour::Blue);
		renderer->SetVsync(false);
		windowInit = true;
	}
	
	float timer = 0.0f;
	if (windowInit)
	{
		while (Engine::Core::Running)
		{
			if (Engine::Core::Update() != EXIT_SUCCESS)
			{
				// Update returned false, exit.
				break;
			}

			if (Engine::Core::Render() != EXIT_SUCCESS)
			{
				// Rendering failed, exit.
				break;
			}

			timer += Engine::Time::DeltaTime();
			if (timer > 1.0f)
			{
				std::stringstream fps;
				fps << "Engine - " << Engine::Time::FrameRate() << " FPS";
				renderer->SetWindowTitle(fps.str());
				timer -= 1.0f;
			}
		}
	}

	Engine::Logging::Log("\n\nDestroying engine...");

	Engine::Core::Destroy();

	Engine::Logging::Log("Engine destroyed");

	return EXIT_SUCCESS;
}
