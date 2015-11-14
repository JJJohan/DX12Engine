#pragma once

#include <string>
#include "CPU\Processor.h"

namespace Engine
{
	class Vector2;

	class SystemInfo
	{
	public:
		static void PrintSystemInfo();
		static Vector2 GetNativeResolution();

	private:
		static std::string GetOSInfo();
		static std::string GetCPUInfo();
		static std::string GetGPUInfo();
		static std::string GetMemInfo();
		static std::string GetDisplayInfo();
		static std::string GetFileVersion(std::string fileName);
		static Processor _processor;
	};
}