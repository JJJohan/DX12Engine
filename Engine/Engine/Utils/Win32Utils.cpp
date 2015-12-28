#include "Win32Utils.h"
#include <Windows.h>
#include "../Rendering/Renderer.h"

namespace Engine
{
	int Win32Utils::ShowMessageBox(std::string message, std::string title, MessageBoxType messageType)
	{
		HWND handle = Renderer::GetWindowHandle();
		return MessageBoxEx(handle, message.c_str(), title.c_str(), 0, 0);
	}
}