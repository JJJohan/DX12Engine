#pragma once

namespace Engine
{
	class Vector2;
	class Colour;

	class Console
	{
	public:
		enum ConsoleColour
		{
			Current = -1,
			Black = 0,
			DarkBlue = 1,
			DarkGreen = 2,
			DarkAqua,
			DarkCyan = 3,
			DarkRed = 4,
			DarkPurple = 5,
			DarkPink = 5,
			DarkMagenta = 5,
			DarkYellow = 6,
			DarkWhite = 7,
			Gray = 8,
			Blue = 9,
			Green = 10,
			Aqua = 11,
			Cyan = 11,
			Red = 12,
			Purple = 13,
			Pink = 13,
			Magenta = 13,
			Yellow = 14,
			White = 15
		};

		static void InitConsole();
		static void SetColour(ConsoleColour textColour, ConsoleColour backgroundColour = ConsoleColour::Current);
		static ConsoleColour GetTextColour();
		static ConsoleColour GetBackgroundColour();
		static Vector2 GetCursorPos();
		static void SetCursorPos(int x, int y);

	private:
		static bool HandlerRoutine(DWORD ctrlType);
		static int _textColour;
		static int _backgroundColour;
	};
}

