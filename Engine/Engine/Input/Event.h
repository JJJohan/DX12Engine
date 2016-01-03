#pragma once
#include <functional>
#include <vector>

// Taken from http://geekswithblogs.net/raccoon_tim/archive/2011/09/28/lambdas-and-events-in-c.aspx
// Simple events for C++11 onwards.

namespace Engine
{
	class Event
	{
	public:
		typedef std::function<void()> Func;
		int ID() const;

		void Call();
		void operator ()();
		Event& operator +=(const Func& f);
		Event& operator -=(const Func& f);
		Event();
	private:
		std::vector<Func> _handlers;
		int _id;
	};
}

