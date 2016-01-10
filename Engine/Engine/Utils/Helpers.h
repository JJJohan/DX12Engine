#pragma once

#include <comdef.h>
#include "Logging.h"
#include "../Core/Core.h"

static bool Failed(HRESULT task)
{
	if (task < 0)
	{
		_com_error err(task);
		std::wstring wide = std::wstring(err.ErrorMessage());
		std::string errorString(wide.begin(), wide.end());
		Engine::Logging::Log(errorString);

		return true;
	}

	return false;
}

static std::string GetRelativeFilePath(const char* relativePath)
{
	std::string appDirectory = Engine::Core::GetApplicationDirectory();
	appDirectory.append(relativePath);
	return appDirectory;
}

static std::wstring GetRelativeFilePath(const wchar_t* relativePath)
{
	std::string appDirectory = Engine::Core::GetApplicationDirectory();
	std::wstring wide = std::wstring(appDirectory.begin(), appDirectory.end());
	wide.append(relativePath);
	return wide;
}

