#pragma once

#include "../stdafx.h"

namespace Engine
{
	class Core;
	struct ENGINE_LINK_DESC;
}

namespace EngineNET
{
	ref class String;

	public delegate void StartFunc();
	public delegate void UpdateFunc();
	public delegate void DrawFunc();
	public delegate void DestroyFunc();

	public ref class Core
	{
	public:
		~Core();
		!Core();

		static bool Update();
		static String^ GetApplicationDirectory();
		static bool Running();
		static void Exit();
		static void Destroy();
		static void Initialise(int width, int height, bool windowed, StartFunc^ start, UpdateFunc^ update, DrawFunc^ draw, DestroyFunc^ destroy);

	private:
		Core();
		static Engine::Core* _pCore;
	};
}