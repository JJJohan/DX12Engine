#include "Helpers.h"
#include "../Data/String.h"
#include "Engine/Utils/Helpers.h"

namespace EngineNET
{
	System::String^ Helpers::GetRelativePath(System::String^ relativePath)
	{
		return String::Std2Sys(Engine::GetRelativePath(String::Sys2Std(relativePath).c_str()));
	}
}