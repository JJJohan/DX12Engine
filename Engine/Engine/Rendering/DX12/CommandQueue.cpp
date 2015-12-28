#include "CommandQueue.h"

namespace Engine
{
	std::queue<std::function<void()>> CommandQueue::_tasks;

	void CommandQueue::Enqueue(std::function<void()> task)
	{
		_tasks.push(task);
	}

	void CommandQueue::Process()
	{
		while (!_tasks.empty())
		{
			std::function<void()> task = _tasks.front();
			task();
			_tasks.pop();
		}
	}
}