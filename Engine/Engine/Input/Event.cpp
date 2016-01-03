#include "Event.h"

namespace Engine
{
	static int eventId = 0;

	Event::Event()
		: _id(eventId++)
	{
	}

	int Event::ID() const
	{
		return _id;
	}

	void Event::Call()
	{
		for (auto i = _handlers.begin(); i != _handlers.end(); ++i)
		{
			(*i)();
		}
	}

	void Event::operator ()()
	{
		Call();
	}

	Event& Event::operator +=(const Func& f)
	{
		_handlers.push_back(f);
		return *this;
	}

	Event& Event::operator -=(const Func& f)
	{
		for (auto i = _handlers.begin(); i != _handlers.end(); ++i)
		{
			if ((*i).target<void>() == f.target<void>())
			{
				_handlers.erase(i);
				break;
			}
		}

		return *this;
	}
}

