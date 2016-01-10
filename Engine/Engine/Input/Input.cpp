#include "Input.h"

namespace Engine
{
	std::map<int, std::vector<Input::KeyEvent>> Input::_keyEvents;
	std::map<int, std::vector<Input::MouseEvent>> Input::_mouseEvents;
	std::map<int, bool> Input::_activeKeys;
	std::map<int, bool> Input::_activeMouseButtons;
	std::vector<std::function<void(long x, long y)>> Input::_mouseMoveEvents;

	void Input::KeyUpEvent(unsigned short keyCode)
	{
		_activeKeys[int(keyCode)] = false;

		CallKeyEvents(int(keyCode), false);
	}

	void Input::KeyDownEvent(unsigned short keyCode)
	{
		CallKeyEvents(int(keyCode), true);

		_activeKeys[int(keyCode)] = true;
	}

	void Input::MouseButtonUpEvent(unsigned short mouseButton)
	{
		_activeMouseButtons[int(mouseButton)] = false;

		CallMouseEvents(int(mouseButton), false);
	}

	void Input::MouseButtonDownEvent(unsigned short mouseButton)
	{
		CallMouseEvents(int(mouseButton), true);

		_activeMouseButtons[int(mouseButton)] = true;
	}

	void Input::MouseMoveEvent(long deltaX, long deltaY)
	{
		for (auto it = _mouseMoveEvents.begin(); it != _mouseMoveEvents.end(); ++it)
		{
			(*it)(deltaX, deltaY);
		}
	}

	void Input::CallKeyEvents(int keyCode, bool keyPressed)
	{
		auto it = _keyEvents.find(keyCode);
		if (it != _keyEvents.end())
		{
			std::vector<KeyEvent> events = it->second;
			for (size_t i = 0; i < events.size(); ++i)
			{
				if ((events[i].KeyState == KeyUp && keyPressed)
					|| (events[i].KeyState == KeyDown && !keyPressed))
				{
					events[i].Event();
				}
			}
		}
	}

	void Input::CallMouseEvents(int mouseButton, bool buttonPressed)
	{
		auto it = _mouseEvents.find(mouseButton);
		if (it != _mouseEvents.end())
		{
			std::vector<MouseEvent> events = it->second;
			for (size_t i = 0; i < events.size(); ++i)
			{
				if ((events[i].MouseState == ButtonUp && buttonPressed)
					|| (events[i].MouseState == ButtonDown && !buttonPressed))
				{
					events[i].Event();
				}
			}
		}
	}

	void Input::Update()
	{
		for (auto it = _activeKeys.begin(); it != _activeKeys.end(); ++it)
		{
			// Check if key is held down
			if (it->second)
			{
				if (_keyEvents.find(it->first) != _keyEvents.end())
				{
					for (auto eventIter = _keyEvents[it->first].begin(); eventIter != _keyEvents[it->first].end(); ++eventIter)
					{
						eventIter->Event();
					}
				}
			}
		}

		for (auto it = _activeMouseButtons.begin(); it != _activeMouseButtons.end(); ++it)
		{
			// Check if mouse button is held down
			if (it->second)
			{
				if (_mouseEvents.find(it->first) != _mouseEvents.end())
				{
					for (auto eventIter = _mouseEvents[it->first].begin(); eventIter != _mouseEvents[it->first].end(); ++eventIter)
					{
						eventIter->Event();
					}
				}
			}
		}
	}

	void Input::RegisterKey(int keyCode, KeyState keyState, const std::function<void()>& event)
	{
		KeyEvent keyEvent;
		keyEvent.Event = event;
		keyEvent.KeyState = keyState;

		auto it = _keyEvents.find(keyCode);
		if (it == _keyEvents.end())
		{
			_keyEvents[keyCode] = std::vector<KeyEvent>();
		}

		_keyEvents[keyCode].push_back(keyEvent);
	}

	void Input::RegisterMouseButton(int mouseButton, MouseState mouseState, const std::function<void()>& event)
	{
		MouseEvent mouseEvent;
		mouseEvent.Event = event;
		mouseEvent.MouseState = mouseState;

		auto it = _mouseEvents.find(mouseButton);
		if (it == _mouseEvents.end())
		{
			_mouseEvents[mouseButton] = std::vector<MouseEvent>();
		}

		_mouseEvents[mouseButton].push_back(mouseEvent);
	}

	void Input::UnregisterMouseButton(int mouseButton, MouseState mouseState, const std::function<void()>& event)
	{
		auto it = _mouseEvents.find(mouseButton);
		if (it != _mouseEvents.end())
		{
			for (auto eventIter = _mouseEvents[mouseButton].begin(); eventIter != _mouseEvents[mouseButton].end(); ++eventIter)
			{
				if (eventIter->MouseState == mouseButton && eventIter->Event.target<void>() == event.target<void>())
				{
					_mouseEvents[mouseButton].erase(eventIter);
					return;
				}
			}
		}
	}

	void Input::RegisterMouseMoveEvent(const std::function<void(long x, long y)>& event)
	{
		_mouseMoveEvents.push_back(event);
	}

	void Input::UnregisterMouseMoveEvent(const std::function<void(long x, long y)>& event)
	{
		for (auto it = _mouseMoveEvents.begin(); it != _mouseMoveEvents.end(); ++it)
		{
			if (it->target<void>() == event.target<void>())
			{
				_mouseMoveEvents.erase(it);
				return;
			}
		}
	}

	void Input::UnregisterKey(int keyCode, KeyState keyState, const std::function<void()>& event)
	{
		auto it = _keyEvents.find(keyCode);
		if (it != _keyEvents.end())
		{
			for (auto eventIter = _keyEvents[keyCode].begin(); eventIter != _keyEvents[keyCode].end(); ++eventIter)
			{
				if (eventIter->KeyState == keyState && eventIter->Event.target<void>() == event.target<void>())
				{
					_keyEvents[keyCode].erase(eventIter);
					return;
				}
			}
		}
	}
}

