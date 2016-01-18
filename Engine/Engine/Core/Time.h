#pragma once

namespace Engine
{
	class DX12Renderer;
	class Core;

	class Time
	{
	public:
		static float DeltaTime();
		static float RunningTime();
		static float GPUTime();

	private:
		static void Initialise();
		static void ResetCPUCounter();

		static void Update();
		static void SetGPUDelta(float delta);

		static float _deltaTime;
		static float _runningTime;
		static float _gpuTime;
		static float _frequency;
		static __int64 _cpuCounter;

		friend Core;
		friend DX12Renderer;
	};
}

