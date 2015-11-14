#include <iostream>
#include <windows.h>
#include "Logging.h"
#ifdef _DEBUG
#include "Console.h"
#endif

namespace Engine
{
	Logging::LogPriority Logging::LogLevel = Logging::LogPriority::Info;

	void Logging::LogError(std::string message)
	{
		LogLevel = LogPriority::Error;
		Log(message);
	}

	void Logging::LogWarning(std::string message)
	{
		LogLevel = LogPriority::Warning;
		Log(message);
	}

	void Logging::Log(std::string message)
	{
#ifdef _DEBUG
		Console::ConsoleColour currentColour = Console::GetTextColour();
#endif

		if (LogLevel == LogPriority::Error)
		{
			message = "[ERROR] " + message;
#ifdef _DEBUG
			Console::SetColour(Console::ConsoleColour::Red);
#endif
		}
		else if (LogLevel == LogPriority::Warning)
		{
			message = "[WARNING] " + message;

#ifdef _DEBUG
			Console::SetColour(Console::ConsoleColour::Yellow);
#endif
		}

		std::cout << message << std::endl;

#ifdef _DEBUG
		std::string output = message + "\n";
		OutputDebugString(output.c_str());
		Console::SetColour(currentColour);
#endif
		LogLevel = LogPriority::Info;
	}

	void Logging::LogWin32Error()
	{
		DWORD errorCode = GetLastError();
		if (errorCode != 0)
		{
			std::string errorString = GetWin32ErrorString(errorCode);
			std::cout << "Error " << errorCode << ": " << errorString << std::endl;
		}
	}

	//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	std::string Logging::GetWin32ErrorString(DWORD errorCode)
	{
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), LPSTR(&messageBuffer), 0, nullptr);

		std::string message(messageBuffer, size);

		//Free the buffer.
		LocalFree(messageBuffer);

		return message;
	}
}
