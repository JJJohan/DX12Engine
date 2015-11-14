#pragma once
#include <errhandlingapi.h>
#include <string>

namespace Engine
{
	class Logging
	{
	public:
		static void Log(std::string message);
		static void LogError(std::string message);
		static void LogWarning(std::string message);
		static void LogWin32Error();

	private:
		enum LogPriority
		{
			Info,
			Warning,
			Error
		};

		static std::string GetWin32ErrorString(DWORD errorCode);
		static LogPriority LogLevel;
	};
}
