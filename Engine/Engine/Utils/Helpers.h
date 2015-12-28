#pragma once

#include <comdef.h>
#include <string>
#include "Logging.h"
#include "../Core/Core.h"

static bool Failed(HRESULT task)
{
	if (task < 0)
	{
		_com_error err(task);
		std::string errorString(err.ErrorMessage());
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