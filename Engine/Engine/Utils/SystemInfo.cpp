#include <Windows.h>
#include <VersionHelpers.h>
#include <sstream>
#include "SystemInfo.h"
#include "Logging.h"
#include "StringUtils.h"
#include "..\Data\Vector2.h"
#include "../Rendering/Renderer.h"
#ifdef _DEBUG
#include "Console.h"
#endif

namespace Engine
{
	Processor SystemInfo::_processor = Processor();

	void SystemInfo::PrintSystemInfo()
	{
		// Ignore logging this to file if GPU info is not available (yet?).
		bool ignored = false;
		if (Renderer::GetRenderer() == nullptr)
		{
			Logging::EnableFileLogging(false);
			ignored = true;
		}

#ifdef _DEBUG
		Console::ConsoleColour textColour = Console::GetTextColour();
		Console::SetColour(Console::ConsoleColour::Cyan);
		Vector2 cursorPos = Console::GetCursorPos();
		Console::SetCursorPos(0, 2);
#endif

		Logging::Log("----------------------");
		Logging::Log("SYSTEM INFO");

		// OS
		Logging::Log(GetOSInfo());

		// CPU
		Logging::Log(GetCPUInfo());

		// GPU
		Logging::Log(GetGPUInfo());

		// Display
		Logging::Log(GetDisplayInfo());

		// Memory
		Logging::Log(GetMemInfo());

		Logging::Log("----------------------");

#ifdef _DEBUG
		Console::SetColour(textColour);

		if (cursorPos.Y != 2)
		{
			Console::SetCursorPos(int(cursorPos.X), int(cursorPos.Y));
		}
#endif

		// Make sure logging is re-enabled if it was hidden.
		if (ignored)
		{
			Logging::EnableFileLogging(true);
		}
	}

	Vector2 SystemInfo::GetNativeResolution()
	{
		return Vector2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	}

	std::string SystemInfo::GetOSInfo()
	{
		// Get the Windows build number from Kernel32.dll
		std::string buildNumber = GetFileVersion("Kernel32.dll");
		std::vector<std::string> versionSplit = Split(buildNumber, '.');
		int major = stoi(versionSplit[0]);
		int minor = stoi(versionSplit[1]);

		// Fetch OS info
		std::string windowsName;
		if (IsWindowsServer())
		{
			windowsName = "OS: Windows Server Variant";
		}
		else if (major > 10 || (major == 10 && minor > 0))
		{
			windowsName = "OS: Newer than Windows 10";
		}
		else if (major == 10 && minor == 0)
		{
			windowsName = "OS: Windows 10";
		}
		else if (major == 6 && minor == 3)
		{
			windowsName = "OS: Windows 8.1";
		}
		else if (major == 6 && minor == 2)
		{
			windowsName = "OS: Windows 8";
		}
		else if (major == 6 && minor == 1)
		{
			windowsName = "OS: Windows 7";
		}
		else if (major == 6 && minor == 0)
		{
			windowsName = "OS: Windows Vista";
		}
		else
		{
			windowsName = "OS: Unknown";
		}

		// Windows 32 or 64-bit architecture check		
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		std::string architecture;
		if (sysInfo.wProcessorArchitecture == 0)
		{
			architecture = " 32-bit ";
		}
		else if (sysInfo.wProcessorArchitecture == 9)
		{
			architecture = " 64-bit ";
		}
		else
		{
			architecture = " with unknown architecture ";
		}

		return windowsName + architecture + "(Build: " + buildNumber + ")";
	}

	std::string SystemInfo::GetCPUInfo()
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		std::stringstream ss;
		ss << "CPU: " << _processor.GetName() << "\n";
		ss << "  Estimated Clock Speed: " << _processor.GetClockSpeed() << "Mhz\n";
		ss << "  Features: " << _processor.GetFeatures() << "\n";
		ss << "  Logical Cores: " << _processor.GetLogicalCores();
		return ss.str();
	}

	std::string SystemInfo::GetGPUInfo()
	{
		std::stringstream ss;
		ss.precision(3);
		IRenderer* renderer = Renderer::GetRenderer();
		if (renderer != nullptr)
		{
			float memTotal = float(renderer->GetDeviceMemoryTotal());
			float memFree = float(renderer->GetDeviceMemoryFree());
			ss << "GPU: " << renderer->GetDeviceName() << "\n";
			ss << "  Dedicated Memory: " << memTotal / 1024 << " GB (" << size_t(memTotal) << " MB)\n";
			ss << "  Free Memory: " << memFree / 1024 << " GB (" << size_t(memFree) << " MB)\n";
			ss << "  Feature Level: " << renderer->GetMaxFeatureLevel();
		}
		else
		{
			ss << "GPU: Unavailable" << "\n";
			ss << "  Dedicated Memory: Unavailable\n";
			ss << "  Free Memory: Unavailable\n";
			ss << "  Feature Level: Unavailable";
		}

		return ss.str();
	}

	int SystemInfo::GetCPUCores()
	{
		return _processor.GetLogicalCores();
	}

	std::string SystemInfo::GetMemInfo()
	{
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof memInfo;
		GlobalMemoryStatusEx(&memInfo);

		DWORDLONG megabytes = memInfo.ullTotalPhys / 1024 / 1024;
		double gigabytes = double(megabytes) / 1024.0f;

		std::stringstream ss;
		ss.precision(3);
		ss << "MEM: " << gigabytes << " GB (" << megabytes << " MB)";
		return ss.str();
	}

	std::string SystemInfo::GetDisplayInfo()
	{
		Vector2 resolution = GetNativeResolution();
		std::stringstream ss;
		ss << "  Display: " << int(resolution.X) << "x" << int(resolution.Y);
		return ss.str();
	}

	std::string SystemInfo::GetFileVersion(const std::string& fileName)
	{
		std::wstring wide = std::wstring(fileName.begin(), fileName.end());
		DWORD verHandle = NULL;
		UINT size = 0;
		LPBYTE lpBuffer = NULL;
		DWORD verSize = GetFileVersionInfoSize(wide.c_str(), &verHandle);
		std::stringstream versionStream;

		if (verSize != NULL)
		{
			LPSTR verData = new char[verSize];

			if (GetFileVersionInfo(wide.c_str(), verHandle, verSize, verData))
			{
				if (VerQueryValue(verData, L"\\", (VOID FAR* FAR*)&lpBuffer, &size))
				{
					if (size)
					{
						VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
						if (verInfo->dwSignature == 0xfeef04bd)
						{
							// Doesn't matter if you are on 32 bit or 64 bit,
							// DWORD is always 32 bits, so first two revision numbers
							// come from dwFileVersionMS, last two come from dwFileVersionLS
							versionStream << ((verInfo->dwFileVersionMS >> 16) & 0xffff)
								<< "." << ((verInfo->dwFileVersionMS >> 0) & 0xffff)
								<< "." << ((verInfo->dwFileVersionLS >> 16) & 0xffff)
								<< "." << ((verInfo->dwFileVersionLS >> 0) & 0xffff);
						}
					}
				}
			}
			delete[] verData;
		}

		return versionStream.str();
	}
}

