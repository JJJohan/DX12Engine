#include "Input.h"

namespace Engine
{
	std::map<int, std::vector<Input::KeyEvent>> Input::_keyEvents;

	void Input::KeyUpEvent(unsigned long long keyCode)
	{
		CallEvents(int(keyCode), false);
	}

	void Input::KeyDownEvent(unsigned long long keyCode)
	{
		CallEvents(int(keyCode), true);
	}

	void Input::CallEvents(int keyCode, bool keyPressed)
	{
		auto it = _keyEvents.find(keyCode);
		if (it != _keyEvents.end())
		{
			std::vector<KeyEvent> events = it->second;
			for (int i = 0; i < events.size(); ++i)
			{
				if (events[i].KeyPressed == keyPressed)
				{
					events[i].Event();
				}
			}
		}
	}

	void Input::RegisterKey(int keyCode, KeyState keyState, std::function<void()> event)
	{
		KeyEvent keyEvent;
		keyEvent.Event = event;
		keyEvent.KeyPressed = (keyState == KeyDown) ? true : false;

		auto it = _keyEvents.find(keyCode);
		if (it == _keyEvents.end())
		{
			_keyEvents[keyCode] = std::vector<KeyEvent>();
		}

		_keyEvents[keyCode].push_back(keyEvent);
	}

	void Input::UnregisterKey(int keyCode, KeyState keyState, std::function<void()> event)
	{
		bool keyPressed = (keyState == KeyDown) ? true : false;

		auto it = _keyEvents.find(keyCode);
		if (it != _keyEvents.end())
		{
			for (auto eventIter = _keyEvents[keyCode].begin(); eventIter != _keyEvents[keyCode].end(); ++eventIter)
			{
				if (eventIter->KeyPressed == keyPressed && eventIter->Event.target<void()>() == event.target<void()>())
				{
					_keyEvents[keyCode].erase(eventIter);
					return;
				}
			}
		}
	}
}
