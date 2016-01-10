#include <windows.h>
#include <intrin.h>
#include <sstream>
#include "Processor.h"

namespace Engine
{
	static const unsigned PROCESSOR_UNKNOWN = 0;
	static const unsigned PROCESSOR_AMD = 1;
	static const unsigned PROCESSOR_INTEL = 2;

	//============================================================================

	// Read the CPU speed from the registry

	//============================================================================
	DWORD ReadCPUSpeedFromRegistry(DWORD dwCPU)
	{
		HKEY hKey;
		DWORD dwSpeed;

		// Get the key name
		std::stringstream ss;
		ss << "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\" << dwCPU << "\\";
		std::string szKey = ss.str();
		std::wstring szKeyW = std::wstring(szKey.begin(), szKey.end());

		// Open the key
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKeyW.c_str(), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
		{
			return 0;
		}

		// Read the value
		DWORD dwLen = 4;
		if (RegQueryValueEx(hKey, L"~MHz", nullptr, nullptr, LPBYTE(&dwSpeed), &dwLen) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return 0;
		}

		// Cleanup and return
		RegCloseKey(hKey);
		return dwSpeed;
	}

	//============================================================================

	// Calculate and log the CPU speed and features

	//============================================================================

	Processor::Processor()
	{
		Refresh();
	}

	void Processor::Refresh()
	{
		unsigned nHighestFeature;
		unsigned nHighestFeatureEx;
		int nBuff[4];
		char szMan[13];
		std::stringstream features;
		unsigned nProcessorType;

		// Get CPU manufacturer and highest CPUID
		__cpuid(nBuff, 0);
		nHighestFeature = unsigned(nBuff[0]);
		*reinterpret_cast<int*>(&szMan[0]) = nBuff[1];
		*reinterpret_cast<int*>(&szMan[4]) = nBuff[3];
		*reinterpret_cast<int*>(&szMan[8]) = nBuff[2];
		szMan[12] = 0;

		if (strcmp(szMan, "AuthenticAMD") == 0)
			nProcessorType = PROCESSOR_AMD;
		else if (strcmp(szMan, "GenuineIntel") == 0)
			nProcessorType = PROCESSOR_INTEL;
		else
			nProcessorType = PROCESSOR_UNKNOWN;

		// Get highest extended feature
		__cpuid(nBuff, 0x80000000);
		nHighestFeatureEx = unsigned(nBuff[0]);

		// Get processor brand name
		if (nHighestFeatureEx >= 0x80000004)
		{
			char szCPUName[49];
			szCPUName[0] = 0;
			__cpuid(reinterpret_cast<int*>(&szCPUName[0]), 0x80000002);
			__cpuid(reinterpret_cast<int*>(&szCPUName[16]), 0x80000003);
			__cpuid(reinterpret_cast<int*>(&szCPUName[32]), 0x80000004);
			szCPUName[48] = 0;

			for (int i = int(strlen(szCPUName)) - 1; i >= 0; --i)
			{
				if (szCPUName[i] == ' ')
					szCPUName[i] = '\0';
				else
					break;
			}

			std::stringstream cpuNameStream;
			cpuNameStream << szCPUName << " (" << szMan << ")";
			_name = cpuNameStream.str();
		}
		else
		{
			_name = szMan;
		}

		// Get CPU features
		if (nHighestFeature >= 1)
		{
			__cpuid(nBuff, 1);
			if (nBuff[3] & 1 << 0)
				features << "FPU ";
			if (nBuff[3] & 1 << 23)
				features << "MMX ";
			if (nBuff[3] & 1 << 25)
				features << "SSE ";
			if (nBuff[3] & 1 << 26)
				features << "SSE2 ";
			if (nBuff[2] & 1 << 0)
				features << "SSE3 ";

			// Intel specific:
			if (nProcessorType == PROCESSOR_INTEL)
			{
				if (nBuff[2] & 1 << 9)
					features << "SSSE3 ";
				if (nBuff[2] & 1 << 7)
					features << "EST ";
			}

			if (nBuff[3] & 1 << 28)
				features << "HTT ";
		}

		// AMD specific:
		if (nProcessorType == PROCESSOR_AMD)
		{
			// Get extended features
			__cpuid(nBuff, 0x80000000);
			if (nHighestFeatureEx >= 0x80000001)
			{
				__cpuid(nBuff, 0x80000001);
				if (nBuff[3] & 1 << 31)
					features << "3DNow! ";
				if (nBuff[3] & 1 << 30)
					features << "Ex3DNow! ";
				if (nBuff[3] & 1 << 22)
					features << "MmxExt ";
			}

			// Get level 1 cache size
			if (nHighestFeatureEx >= 0x80000005)
			{
				__cpuid(nBuff, 0x80000005);
				_l1Cache = unsigned(nBuff[2]) >> 24;
			}
		}

		// Get cache size
		if (nHighestFeatureEx >= 0x80000006)
		{
			__cpuid(nBuff, 0x80000006);
			_l2Cache = unsigned(nBuff[2]) >> 16;
		}

		// Log features
		_features = features.str();

		// Get misc system info
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		// Log number of CPUs and speeds
		_cores = sysInfo.dwNumberOfProcessors;
		_frequency = ReadCPUSpeedFromRegistry(0);
	}

	std::string Processor::GetName() const
	{
		return _name;
	}

	std::string Processor::GetFeatures() const
	{
		return _features;
	}

	int Processor::GetL1CacheSize() const
	{
		return _l1Cache;
	}

	int Processor::GetL2CacheSize() const
	{
		return _l2Cache;
	}

	int Processor::GetLogicalCores() const
	{
		return _cores;
	}

	int Processor::GetFrequency() const
	{
		return _frequency;
	}

	double Processor::GetClockSpeed()
	{
		LARGE_INTEGER qwWait, qwStart, qwCurrent;
		QueryPerformanceCounter(&qwStart);
		QueryPerformanceFrequency(&qwWait);
		qwWait.QuadPart >>= 5;
		unsigned __int64 Start = __rdtsc();
		do
		{
			QueryPerformanceCounter(&qwCurrent);
		}
		while (qwCurrent.QuadPart - qwStart.QuadPart < qwWait.QuadPart);
		return ((__rdtsc() - Start) << 5) / 1000000.0;
	}
}

