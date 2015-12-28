#pragma once

#include <queue>
#include <functional>

namespace Engine
{
	class CommandQueue
	{
	public:
		static void Enqueue(std::function<void()> task);
		static void Process();

	private:
		static std::queue<std::function<void()>> _tasks;
	};
}
