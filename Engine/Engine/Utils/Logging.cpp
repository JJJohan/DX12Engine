#include "Win32Utils.h"
#include "Helpers.h"

namespace Engine
{
	Logging::LogPriority Logging::LogLevel = Logging::LogPriority::Info;
	bool Logging::_logToFile = true;
	String Logging::_logFilePath = GetRelativePath("");

	void Logging::LogError(const String& message)
	{
		LogLevel = LogPriority::Error;
		Log(message);
		Win32Utils::ShowMessageBox(message, "Error");
	}

	void Logging::LogWarning(const String& message)
	{
		LogLevel = LogPriority::Warning;
		Log(message);
	}

	void Logging::Log(const String& message)
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
	String Logging::GetWin32ErrorString()
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

	//Returns a COM error message.
	String Logging::GetCOMError(const _com_error& error, const String& caller)
	{

		return String(caller + ": " + error.ErrorMessage());
	}

	void Logging::EnableFileLogging(bool enabled)
	{
		_logToFile = enabled;
	}

	void Logging::SetLogPath(const String& filePath)
	{
		_logFilePath = filePath;
	}
}

