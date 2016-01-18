#pragma once

#include <queue>
#include <functional>
#include <mutex>

struct ID3D12CommandList;
struct ID3D12CommandAllocator;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;

namespace Engine
{
	struct CommandThread
	{
		ID3D12CommandList* CommandList;
		ID3D12CommandAllocator* CommandAllocator;
		std::thread Thread;
		std::mutex Mutex;
		bool Available;
		bool Open;
		int TasksCompleted;
		std::function<void()> Task;
		float IdleTimer;
		bool IdleTimeOut;
	};

	class CommandQueue
	{
	public:
		static void Enqueue(const std::function<void()>& task);

		static std::vector<ID3D12CommandList*> CommandQueue::Process(ID3D12Device* device);
		static void Release();

	private:
		static void RunTask(CommandThread* commandThread);
		static inline CommandThread* GetAvailableThread();

		static std::queue<std::function<void()>> _tasks;
		static std::vector<CommandThread*> _commandThreads;
		static std::mutex _releaseMutex;
		static bool _releaseRequested;

		static ID3D12CommandAllocator* _commandAllocator;
		static ID3D12GraphicsCommandList* _commandList;
	};
}

