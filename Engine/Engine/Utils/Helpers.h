#pragma once

#include <comdef.h>
#include <string>
#include "Logging.h"

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
