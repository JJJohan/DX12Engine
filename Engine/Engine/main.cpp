#include <iostream>
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

	Engine::Logging::Log("Starting engine...\n");

	Engine::SystemInfo::PrintSystemInfo();

	Engine::IRenderer* renderer = Engine::Renderer::CreateRenderer(Engine::RenderAPI::Direct3D12);
	bool windowInit = false;
	if (renderer->InitWindow(1024, 768, true) != EXIT_FAILURE)
	{
		renderer->SetClearColour(Engine::Colour::Blue);
		renderer->SetVsync(false);
		windowInit = true;
	}

	Engine::SystemInfo::PrintSystemInfo();

	Engine::Event test;
	test += [] { Engine::Logging::LogError("Whoah!"); };

	Engine::Input::RegisterKey(VK_F1, Engine::KeyDown, [] { Engine::Logging::LogError("Whoah!"); });
	Engine::Input::RegisterKey(VK_F2, Engine::KeyUp, [] { Engine::Logging::LogError("Whoah!"); });

	float timer = 0.0f;
	if (windowInit)
	{
		Engine::Core::BeginRendering();
		while (Engine::Core::Running())
		{
			if (Engine::Core::Update() != EXIT_SUCCESS)
			{
				// Update returned false, exit.
				break;
			}

			timer += Engine::Time::DeltaTime();
			if (timer > 1.0f)
			{
				std::stringstream fps;
				fps << "Engine - CPU: " << int(1 / Engine::Time::DeltaTime()) << " FPS | GPU: " << int(1 / Engine::Time::GPUTime()) << " FPS";
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
