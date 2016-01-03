#include <iostream>
#include <windows.h>
#include "Logging.h"
#include "Win32Utils.h"
#include "Helpers.h"
#ifdef _DEBUG
#include "Console.h"
#endif

namespace Engine
{
	Logging::LogPriority Logging::LogLevel = Logging::LogPriority::Info;
	bool Logging::_logToFile = true;
	std::string Logging::_logFilePath = GetRelativeFilePath("");

	void Logging::LogError(const std::wstring& message)
	{
		LogError(std::string(message.begin(), message.end()));
	}

	void Logging::LogError(const std::string& message)
	{
		Win32Utils::ShowMessageBox(message, "Error");

		LogLevel = LogPriority::Error;
		Log(message);
	}

	void Logging::LogWarning(const std::string& message)
	{
		LogLevel = LogPriority::Warning;
		Log(message);
	}

	void Logging::Log(const std::string& message)
	{
#ifdef _DEBUG
		Console::ConsoleColour currentColour = Console::GetTextColour();
#endif

		std::string outString = message;
		if (LogLevel == LogPriority::Error)
		{
			outString = "[ERROR] " + outString;
#ifdef _DEBUG
			Console::SetColour(Console::ConsoleColour::Red);
#endif
		}
		else if (LogLevel == LogPriority::Warning)
		{
			outString = "[WARNING] " + outString;

#ifdef _DEBUG
			Console::SetColour(Console::ConsoleColour::Yellow);
#endif
		}

		std::cout << outString << std::endl;

#ifdef _DEBUG
		std::string output = outString + "\n";
		std::wstring outputW = std::wstring(output.begin(), output.end());
		OutputDebugString(outputW.c_str());
		Console::SetColour(currentColour);
#endif
		LogLevel = LogPriority::Info;
	}

	void Logging::LogWin32Error()
	{
		DWORD errorCode = GetLastError();
		std::string errorString = GetWin32ErrorString();
		if (!errorString.empty())
		{
			std::cout << "Error " << errorCode << ": " << errorString << std::endl;
		}
	}

	//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	std::string Logging::GetWin32ErrorString()
	{
		DWORD errorCode = GetLastError();
		if (errorCode == 0)
		{
			return std::string();
		}

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		                                                          nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), LPWSTR(&messageBuffer), 0, nullptr);

		std::string message(messageBuffer, size);

		//Free the buffer.
		LocalFree(messageBuffer);

		return message;
	}

	void Logging::EnableFileLogging(bool enabled)
	{
		_logToFile = enabled;
	}

	void Logging::SetLogPath(const std::string& filePath)
	{
		_logFilePath = filePath;
	}
}

