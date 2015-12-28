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
		static float const GPUTime();

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
