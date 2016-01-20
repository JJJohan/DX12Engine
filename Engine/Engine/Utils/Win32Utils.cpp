#include "Win32Utils.h"

namespace Engine
{
	int Win32Utils::ShowMessageBox(const std::string& message, const std::string& title, MessageBoxType messageType)
	{
		HWND handle = Renderer::GetWindowHandle();
		std::wstring messageW = std::wstring(message.begin(), message.end());
		std::wstring titleW = std::wstring(title.begin(), title.end());
		return MessageBoxEx(handle, messageW.c_str(), titleW.c_str(), 0, 0);
	}
}

