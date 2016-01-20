#pragma once

#include "CPU\Processor.h"

namespace Engine
{
	class Vector2;

	class SystemInfo
	{
	public:
		static ENGINE_API void PrintSystemInfo();
		static ENGINE_API Vector2 GetNativeResolution();
		static ENGINE_API int GetCPUCores();

	private:
		static std::string GetOSInfo();
		static std::string GetCPUInfo();
		static std::string GetGPUInfo();
		static std::string GetMemInfo();
		static std::string GetDisplayInfo();
		static std::string GetFileVersion(const std::string& fileName);
		static Processor _processor;
	};
}

