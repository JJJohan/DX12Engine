#pragma once
#include <map>
#include <vector>
#include "Event.h"

namespace Engine
{
	enum KeyState
	{
		KeyDown,
		KeyUp
	};

	class Input
	{
	public:
		static void KeyUpEvent(unsigned long long keyCode);
		static void KeyDownEvent(unsigned long long keyCode);

		static void RegisterKey(int KeyCode, KeyState keyState, std::function<void()> event);
		static void UnregisterKey(int KeyCode, KeyState keyState, std::function<void()> event);

	private:
		struct KeyEvent
		{
			bool KeyPressed;
			std::function<void()> Event;
		};

		static void CallEvents(int keyCode, bool keyPressed);

		static std::map<int, std::vector<KeyEvent>> _keyEvents;
	};
}
