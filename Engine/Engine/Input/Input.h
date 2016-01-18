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

		static void RegisterKey(int keyCode, KeyState keyState, const std::function<void()>& event, std::string eventName);
		static void RegisterMouseButton(int mouseButton, MouseState mouseState, const std::function<void()>& event, std::string eventName);
		static void RegisterMouseMoveEvent(const std::function<void(long x, long y)>& event, std::string eventName);

		static void UnregisterKey(std::string eventName);
		static void UnregisterMouseButton(std::string eventName);
		static void UnregisterMouseMoveEvent(std::string eventName);

		static void Update();

	private:
		static bool KeyEventExists(std::string eventName);
		static bool MouseEventExists(std::string eventName);
		static bool MouseMoveEventExists(std::string eventName);

		static void DeleteKey(std::string eventName);
		static void DeleteMouseButton(std::string eventName);
		static void DeleteMouseMoveEvent(std::string eventName);

		struct KeyEvent
		{
			KeyState KeyState;
			std::function<void()> Event;
			std::string EventName;
		};

		struct MouseEvent
		{
			MouseState MouseState;
			std::function<void()> Event;
			std::string EventName;
		};

		struct MMoveEvent
		{
			std::function<void(long x, long y)> Event;
			std::string EventName;
		};

		enum DeleteType
		{
			Delete_Key,
			Delete_Mouse,
			Delete_MMove
		};


		static void CallKeyEvents(int keyCode, bool keyPressed);
		static void CallMouseEvents(int mouseButton, bool buttonPressed);

		static std::map<int, std::vector<KeyEvent>> _keyEvents;
		static std::map<int, bool> _activeKeys;
		static std::map<int, std::vector<MouseEvent>> _mouseEvents;
		static std::map<int, bool> _activeMouseButtons;
		static std::vector<MMoveEvent> _mouseMoveEvents;
		static std::map<DeleteType, std::vector<std::string>> _deleteQueue;
		static bool _deletionQueued;
	};
}

