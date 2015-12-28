#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include "Console.h"
#include "Logging.h"
#include "..\Core\Core.h"

namespace Engine
{
	// maximum mumber of lines the output console should have
	static const WORD MAX_CONSOLE_LINES = 500;
	int Console::_backgroundColour = 0;
	int Console::_textColour = 15;

	void Console::SetColour(ConsoleColour textColour, ConsoleColour backgroundColour)
	{
		short textVal = short(textColour) - 1;
		short backVal = short(backgroundColour);

		if (textColour == ConsoleColour::Current)
		{
			textVal = _textColour - 1;
		}
		else
		{
			_textColour = textVal + 1;
		}

		if (backgroundColour == ConsoleColour::Current)
		{
			backVal = _backgroundColour;
		}
		else
		{
			_backgroundColour = backVal;
		}

		if ((textVal % 16) == (textVal % 16))
		{
			++textVal;
		}

		textVal %= 16; backVal %= 16;
		unsigned short wAttributes = (unsigned(backVal) << 4) | unsigned(textVal);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
	}

	Console::ConsoleColour Console::GetTextColour()
	{
		return ConsoleColour(_textColour);
	}

	Vector2 Console::GetCursorPos()
	{
		CONSOLE_SCREEN_BUFFER_INFO SBInfo;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &SBInfo);
		Vector2 cursorPos = Vector2(SBInfo.dwCursorPosition.X, SBInfo.dwCursorPosition.Y);
		return cursorPos;
	}

	void Console::SetCursorPos(int x, int y)
	{
		COORD coordinates;
		coordinates.X = x;
		coordinates.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinates);
	}

	Console::ConsoleColour Console::GetBackgroundColour()
	{
		return ConsoleColour(_backgroundColour);
	}

	bool Console::HandlerRoutine(DWORD ctrlType)
	{
		Core::Exit();
		Sleep(5000);
		return true;
	}

	void Console::InitConsole()
	{
		int hConHandle;
		intptr_t lStdHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		FILE *fp;

		// allocate a console for this app
		AllocConsole();

		// set the screen buffer to be big enough to let us scroll text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

		// redirect unbuffered STDOUT to the console
		lStdHandle = intptr_t(GetStdHandle(STD_OUTPUT_HANDLE));
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stdout);
		*stdout = *fp;
		setvbuf(stdout, nullptr, _IONBF, 0);

		// redirect unbuffered STDIN to the console
		lStdHandle = intptr_t(GetStdHandle(STD_INPUT_HANDLE));
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "r");
		freopen_s(&fp, "CONIN$", "r", stdin);
		*stdin = *fp;
		setvbuf(stdin, nullptr, _IONBF, 0);

		// redirect unbuffered STDERR to the console
		lStdHandle = intptr_t(GetStdHandle(STD_ERROR_HANDLE));
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stderr);
		*stderr = *fp;
		setvbuf(stderr, nullptr, _IONBF, 0);

		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();

		// Specify a handler to listen for close events
		if (!SetConsoleCtrlHandler(PHANDLER_ROUTINE(HandlerRoutine), true))
		{
			Logging::LogError("Could not set console control handler.");
		}
	}
}