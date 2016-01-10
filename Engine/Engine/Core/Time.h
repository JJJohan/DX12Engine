#pragma once

#include <chrono>

namespace Engine
{
	class Core;

	class Time
	{
	public:
		static float DeltaTime();
		static float RunningTime();
		static float GPUTime();

	private:
		static void Update();
		static void UpdateGPU();

		static float _deltaTime;
		static float _runningTime;
		static float _gpuTime;
		static std::chrono::steady_clock::time_point _previousTime;
		static std::chrono::steady_clock::time_point _previousTimeGPU;

		friend Core;
	};
}

