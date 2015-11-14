#pragma once
#include <chrono>

namespace Engine
{
	class Core;

	class Time
	{
	public:
		static float const DeltaTime();
		static float const RunningTime();
		static float const FrameRate();

	private:
		static void Update();

		static float _deltaTime;
		static float _runningTime;
		static std::chrono::steady_clock::time_point _previousTime;

		friend Core;
	};
}
