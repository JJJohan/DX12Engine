#pragma once

#include "../Data/String.h"

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
		static void Log(const String& message);
		static void LogError(const String& message);
		static void LogWarning(const String& message);
		static void LogWin32Error();
		static String GetWin32ErrorString();
		static void EnableFileLogging(bool enabled);
		static void SetLogPath(const String& filePath);

		template <typename... Args>
		static void Log(const String& string, Args ... args)
		{
			std::string text = fmt::format(string.Str(), args...);
			Log(text);
		}

		template <typename... Args>
		static void LogWarning(const String& string, Args ... args)
		{
			std::string text = fmt::format(string.Str(), args...);
			LogWarning(text);
		}

		template <typename... Args>
		static void LogError(const String& string, Args ... args)
		{
			std::string text = fmt::format(string.Str(), args...);
			LogError(text);
		}

	private:
		enum LogPriority
		{
			Info,
			Warning,
			Error
		};

		static LogPriority LogLevel;
		static bool _logToFile;
		static String _logFilePath;
	};
}

