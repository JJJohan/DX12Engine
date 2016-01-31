#pragma once

namespace Engine
{
	class Time;
}

namespace EngineNET
{
	public ref class Time
	{
	public:
		static float DeltaTime();
		static float RunningTime();
		static float GPUTime();
	};
}