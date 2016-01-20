#pragma once

namespace Engine
{
	enum MessageBoxType
	{
		AbortRetryIgnore = 0x00000002L,
		CancelTryAgainContinue = 0x00000006L,
		Help = 0x00004000L,
		OK = 0x00000000L,
		OKCancel = 0x00000001L,
		RetryCancel = 0x00000005L,
		YesNo = 0x00000004L,
		YesNoCancel = 0x00000003L
	};

	class Win32Utils
	{
	public:
		static int ShowMessageBox(const std::string& message, const std::string& title = "Message", MessageBoxType messageType = OK);
	};
}

