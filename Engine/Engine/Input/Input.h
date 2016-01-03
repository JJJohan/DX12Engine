#pragma once
#include <map>
#include <vector>
#include "Event.h"

namespace Engine
{
	enum KeyState
	{
		KeyDown,
		KeyUp,
		KeyHeld
	};

	enum MouseState
	{
		ButtonDown,
		ButtonUp,
		ButtonHeld
	};

	class Input
	{
	public:
		static void KeyUpEvent(unsigned short keyCode);
		static void KeyDownEvent(unsigned short keyCode);

		static void MouseButtonUpEvent(unsigned short mouseButton);
		static void MouseButtonDownEvent(unsigned short mouseButton);
		static void MouseMoveEvent(long deltaX, long deltaY);

		static void RegisterKey(int keyCode, KeyState keyState, const std::function<void()>& event);
		static void UnregisterKey(int keyCode, KeyState keyState, const std::function<void()>& event);

		static void RegisterMouseButton(int mouseButton, MouseState mouseState, const std::function<void()>& event);
		static void UnregisterMouseButton(int mouseButton, MouseState mouseState, const std::function<void()>& event);

		static void RegisterMouseMoveEvent(const std::function<void(long x, long y)>& event);
		static void UnregisterMouseMoveEvent(const std::function<void(long x, long y)>& event);

		static void Update();

	private:
		struct KeyEvent
		{
			KeyState KeyState;
			std::function<void()> Event;
		};

		struct MouseEvent
		{
			MouseState MouseState;
			std::function<void()> Event;
		};

		static void CallKeyEvents(int keyCode, bool keyPressed);
		static void CallMouseEvents(int mouseButton, bool buttonPressed);

		static std::map<int, std::vector<KeyEvent>> _keyEvents;
		static std::map<int, bool> _activeKeys;
		static std::map<int, std::vector<MouseEvent>> _mouseEvents;
		static std::map<int, bool> _activeMouseButtons;
		static std::vector<std::function<void(long x, long y)>> _mouseMoveEvents;
	};
}

