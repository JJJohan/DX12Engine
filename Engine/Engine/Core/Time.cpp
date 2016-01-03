#include "Time.h"

namespace Engine
{
	float Time::_deltaTime = 0.0f;
	float Time::_runningTime = 0.0f;
	float Time::_gpuTime = 0.0f;
	std::chrono::steady_clock::time_point Time::_previousTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point Time::_previousTimeGPU = std::chrono::steady_clock::now();

	float const Time::DeltaTime()
	{
		return _deltaTime;
	}

	float const Time::RunningTime()
	{
		return _runningTime;
	}

	float const Time::GPUTime()
	{
		return _gpuTime;
	}

	void Time::Update()
	{
		// Calculate frame time
		std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> diff = currentTime - _previousTime;
		_previousTime = currentTime;

		_deltaTime = diff.count();
		_runningTime += _deltaTime;
	}

	void Time::UpdateGPU()
	{
		// Calculate frame time
		std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> diff = currentTime - _previousTimeGPU;
		_previousTimeGPU = currentTime;

		_gpuTime = diff.count();
	}
}

