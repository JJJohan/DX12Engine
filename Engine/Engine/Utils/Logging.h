#pragma once
#include <errhandlingapi.h>
#include <string>

// Checks if a result has failed and logs an error.
#define LOGFAILED(x,y) if(FAILED(x)) { Engine::Logging::LogError(y); }

// Checks if a result has failed and logs the last Win32 error message.
#define LOGFAILEDCOM(x) if(FAILED(x)) { Engine::Logging::LogError(Engine::Logging::GetWin32ErrorString()); }

// Checks if a result has failed, logs an error message and returns a specified return value.
#define LOGFAILEDRETURN(x,y,z) if(FAILED(x)) { Engine::Logging::LogError(y); return z; }

// Checks if a result has failed, logs the last Win32 error message and returns a specified return value.
#define LOGFAILEDCOMRETURN(x,y) if(FAILED(x)) { Engine::Logging::LogError(Engine::Logging::GetWin32ErrorString()); return y; }

namespace Engine
{
	class Logging
	{
	public:
		static void Log(const std::string& message);
		static void LogError(const std::string& message);
		static void LogError(const std::wstring& message);
		static void LogWarning(const std::string& message);
		static void LogWin32Error();
		static std::string GetWin32ErrorString();
		static void EnableFileLogging(bool enabled);
		static void SetLogPath(const std::string& filePath);

	private:
		enum LogPriority
		{
			Info,
			Warning,
			Error
		};

		static LogPriority LogLevel;
		static bool _logToFile;
		static std::string _logFilePath;
	};
}

