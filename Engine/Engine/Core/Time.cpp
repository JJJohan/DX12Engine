namespace Engine
{
	float Time::_deltaTime = 0.0f;
	float Time::_runningTime = 0.0f;
	float Time::_gpuTime = 0.0f;
	float Time::_frequency = 0.0f;
	__int64 Time::_cpuCounter = 0;

	void Time::Initialise()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
		{
			Logging::LogError("QueryPerformanceFrequency failed!");
		}

		_frequency = float(li.QuadPart);

		QueryPerformanceCounter(&li);
		_cpuCounter = li.QuadPart;
	}

	void Time::ResetCPUCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		_cpuCounter = li.QuadPart;
	}

	float Time::DeltaTime()
	{
		if (_deltaTime > 10.0f)
		{
			return 10.0f;
		}

		return _deltaTime;
	}

	float Time::RunningTime()
	{
		return _runningTime;
	}

	float Time::GPUTime()
	{
		return _gpuTime;
	}

	void Time::Update()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);

		// Calculate frame time
		_deltaTime = float(li.QuadPart - _cpuCounter) / _frequency;
		_runningTime += _deltaTime;

		ResetCPUCounter();
	}

	void Time::SetGPUDelta(float delta)
	{
		_gpuTime = delta;
	}
}

