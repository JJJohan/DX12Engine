namespace Engine
{
	std::map<int, std::vector<Input::KeyEvent>> Input::_keyEvents;
	std::map<int, std::vector<Input::MouseEvent>> Input::_mouseEvents;
	std::map<int, bool> Input::_activeKeys;
	std::map<int, bool> Input::_activeMouseButtons;
	std::map<Input::DeleteType, std::vector<std::string>> Input::_deleteQueue;
	std::vector<Input::MMoveEvent> Input::_mouseMoveEvents;
	bool Input::_deletionQueued = false;

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
			it->Event(deltaX, deltaY);
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

		// Cleanup
		if (_deletionQueued)
		{
			for (auto it = _deleteQueue[Delete_Key].begin(); it != _deleteQueue[Delete_Key].end(); ++it)
			{
				DeleteKey(*it);
			}

			for (auto it = _deleteQueue[Delete_Mouse].begin(); it != _deleteQueue[Delete_Mouse].end(); ++it)
			{
				DeleteMouseButton(*it);
			}

			for (auto it = _deleteQueue[Delete_MMove].begin(); it != _deleteQueue[Delete_MMove].end(); ++it)
			{
				DeleteMouseMoveEvent(*it);
			}

			_deletionQueued = false;
		}
	}

	void Input::RegisterKey(int keyCode, KeyState keyState, const std::function<void()>& event, std::string eventName)
	{
		if (KeyEventExists(eventName))
		{
			Logging::LogWarning("Tried to register a key event ({0}) which already exists.", eventName);
			return;
		}

		KeyEvent keyEvent;
		keyEvent.Event = event;
		keyEvent.KeyState = keyState;
		keyEvent.EventName = eventName;

		auto it = _keyEvents.find(keyCode);
		if (it == _keyEvents.end())
		{
			_keyEvents[keyCode] = std::vector<KeyEvent>();
		}

		_keyEvents[keyCode].push_back(keyEvent);
	}

	void Input::RegisterMouseButton(int mouseButton, MouseState mouseState, const std::function<void()>& event, std::string eventName)
	{
		if (MouseEventExists(eventName))
		{
			Logging::LogWarning("Tried to register a mouse button event ({0}) which already exists.", eventName);
			return;
		}

		MouseEvent mouseEvent;
		mouseEvent.Event = event;
		mouseEvent.MouseState = mouseState;
		mouseEvent.EventName = eventName;

		auto it = _mouseEvents.find(mouseButton);
		if (it == _mouseEvents.end())
		{
			_mouseEvents[mouseButton] = std::vector<MouseEvent>();
		}

		_mouseEvents[mouseButton].push_back(mouseEvent);
	}

	void Input::RegisterMouseMoveEvent(const std::function<void(long x, long y)>& event, std::string eventName)
	{
		if (MouseMoveEventExists(eventName))
		{
			Logging::LogWarning("Tried to register a mouse move event ({0}) which already exists.", eventName);
			return;
		}

		MMoveEvent moveEvent;
		moveEvent.Event = event;
		moveEvent.EventName = eventName;

		_mouseMoveEvents.push_back(moveEvent);
	}

	bool Input::KeyEventExists(std::string eventName)
	{
		for (auto it = _keyEvents.begin(); it != _keyEvents.end(); ++it)
		{
			std::vector<KeyEvent>& events = it->second;
			for (auto eventIter = events.begin(); eventIter != events.end(); ++eventIter)
			{
				if (eventIter->EventName == eventName)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool Input::MouseEventExists(std::string eventName)
	{
		for (auto it = _mouseEvents.begin(); it != _mouseEvents.end(); ++it)
		{
			std::vector<MouseEvent>& events = it->second;
			for (auto eventIter = events.begin(); eventIter != events.end(); ++eventIter)
			{
				if (eventIter->EventName == eventName)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool Input::MouseMoveEventExists(std::string eventName)
	{
		for (auto it = _mouseMoveEvents.begin(); it != _mouseMoveEvents.end(); ++it)
		{
			if (it->EventName == eventName)
			{
				return true;
			}
		}

		return false;
	}

	void Input::UnregisterKey(std::string eventName)
	{
		for (auto it = _deleteQueue[Delete_Key].begin(); it != _deleteQueue[Delete_Key].end(); ++it)
		{
			if (*it == eventName)
			{
				Logging::LogWarning("Tried to unregister a key event ({0}) that has already been queued for removal.", eventName);
				return;
			}
		}

		_deleteQueue[Delete_Key].push_back(eventName);
		_deletionQueued = true;
	}

	void Input::UnregisterMouseButton(std::string eventName)
	{
		for (auto it = _deleteQueue[Delete_Mouse].begin(); it != _deleteQueue[Delete_Mouse].end(); ++it)
		{
			if (*it == eventName)
			{
				Logging::LogWarning("Tried to unregister a mouse button event ({0}) that has already been queued for removal.", eventName);
				return;
			}
		}

		_deleteQueue[Delete_Mouse].push_back(eventName);
		_deletionQueued = true;
	}

	void Input::UnregisterMouseMoveEvent(std::string eventName)
	{
		for (auto it = _deleteQueue[Delete_MMove].begin(); it != _deleteQueue[Delete_MMove].end(); ++it)
		{
			if (*it == eventName)
			{
				Logging::LogWarning("Tried to unregister a mouse move event ({0}) that has already been queued for removal.", eventName);
				return;
			}
		}

		_deleteQueue[Delete_MMove].push_back(eventName);
		_deletionQueued = true;
	}

	void Input::DeleteKey(std::string eventName)
	{
		for (auto it = _keyEvents.begin(); it != _keyEvents.end(); ++it)
		{
			std::vector<KeyEvent>& events = it->second;
			for (auto eventIter = events.begin(); eventIter != events.end(); ++eventIter)
			{
				if (eventIter->EventName == eventName)
				{
					events.erase(eventIter);
					if (events.empty())
					{
						_keyEvents.erase(it);
					}
					return;
				}
			}
		}

		Logging::LogWarning("Could not find key event named '{0}'.", eventName);
	}

	void Input::DeleteMouseButton(std::string eventName)
	{
		for (auto it = _mouseEvents.begin(); it != _mouseEvents.end(); ++it)
		{
			std::vector<MouseEvent>& events = it->second;
			for (auto eventIter = events.begin(); eventIter != events.end(); ++eventIter)
			{
				if (eventIter->EventName == eventName)
				{
					events.erase(eventIter);
					if (events.empty())
					{
						_mouseEvents.erase(it);
					}
					return;
				}
			}
		}

		Logging::LogWarning("Could not find mouse button event named '{0}'.", eventName);
	}

	void Input::DeleteMouseMoveEvent(std::string eventName)
	{
		for (auto it = _mouseMoveEvents.begin(); it != _mouseMoveEvents.end(); ++it)
		{
			if (it->EventName == eventName)
			{
				_mouseMoveEvents.erase(it);
				return;
			}
		}

		Logging::LogWarning("Could not find mouse move event named '{0}'.", eventName);
	}
}

