#include <thread>
#include <d3d12.h>
#include "CommandQueue.h"
#include "../../Utils/Logging.h"
#include "../../Factory/ResourceFactory.h"

//#define SINGLE_THREADED

namespace Engine
{
	std::queue<std::function<void()>> CommandQueue::_tasks;
	std::mutex CommandQueue::_releaseMutex;
	volatile bool CommandQueue::_releaseRequested = false;
	std::vector<CommandThread*> CommandQueue::_commandThreads;
	ID3D12CommandAllocator* CommandQueue::_commandAllocator;
	ID3D12GraphicsCommandList* CommandQueue::_commandList;

	void CommandQueue::Enqueue(const std::function<void()>& task)
	{
		_tasks.push(task);
	}

	void CommandQueue::Release()
	{
		_releaseMutex.lock();
		_releaseRequested = true;
		_releaseMutex.unlock();

		_commandAllocator->Release();
		_commandList->Release();

		for (auto it = _commandThreads.begin(); it != _commandThreads.end(); ++it)
		{
			CommandThread* commandThread = *it;
			commandThread->WaitCondition.notify_one();
			commandThread->Thread.join();
			commandThread->CommandList->Release();
			commandThread->CommandAllocator->Release();
			delete commandThread;
		}
	}

	void CommandQueue::RunTask(CommandThread* commandThread)
	{
		ResourceFactory::AssignCommandList(commandThread->CommandList);

		while (!_releaseRequested)
		{
			// Wait until work is available.
			std::unique_lock<std::mutex> waitLock(commandThread->WaitMutex);
			if (!commandThread->Waiting)
			{
				std::lock_guard<std::mutex> lock(commandThread->LockMutex);
				commandThread->Waiting = true;
			}
			commandThread->WaitCondition.wait(waitLock, [&] {return !commandThread->Available || _releaseRequested; });
			if (_releaseRequested)
			{
				return;
			}

			commandThread->Task();

			std::lock_guard<std::mutex> lock(commandThread->LockMutex);
			++commandThread->TasksCompleted;
			commandThread->Available = true;
			commandThread->Task = nullptr;
		}
	}

	CommandThread* CommandQueue::GetAvailableThread()
	{
		for (auto it = _commandThreads.begin(); it != _commandThreads.end(); ++it)
		{
			CommandThread* commandThread = *it;
			if (commandThread->Available)
			{
				return commandThread;
			}
		}

		return nullptr;
	}

	std::vector<ID3D12CommandList*> CommandQueue::Process(ID3D12Device* device)
	{
		std::vector<ID3D12CommandList*> commandLists;

#ifdef SINGLE_THREADED
		if (_commandList == nullptr)
		{
			LOGFAILEDCOM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)));
			LOGFAILEDCOM(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator, nullptr, IID_PPV_ARGS(&_commandList)));
		}
		else
		{
			_commandAllocator->Reset();
			_commandList->Reset(_commandAllocator, nullptr);
		}

		ResourceFactory::AssignCommandList(_commandList);
		while (!_tasks.empty())
		{
			std::function<void()> task = _tasks.front();
			task();
			_tasks.pop();
		}
		_commandList->Close();
		commandLists.push_back(_commandList);
#else
		// Create resources for main thread.
		if (_commandList == nullptr)
		{
			LOGFAILEDCOM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)));
			LOGFAILEDCOM(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator, nullptr, IID_PPV_ARGS(&_commandList)));
			_commandList->Close();
		}

		// Begin work load.
		int taskCount = int(_tasks.size() - 1);
		while (_tasks.size() > 1)
		{
			CommandThread* commandThread = GetAvailableThread();
			if (commandThread == nullptr)
			{
				if (_commandThreads.size() < 2)
				{
					std::function<void()> task = _tasks.front();
					_tasks.pop();

					// Create the command list.
					commandThread = new CommandThread;
					commandThread->Available = false;
					commandThread->TasksCompleted = 0;
					commandThread->Task = task;
					commandThread->Open = true;
					commandThread->Waiting = false;

					ID3D12CommandAllocator* commandAllocator;
					ID3D12GraphicsCommandList* commandList;
					LOGFAILEDCOM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
					LOGFAILEDCOM(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
					commandThread->CommandAllocator = commandAllocator;
					commandThread->CommandList = commandList;
					commandThread->Thread = std::thread(RunTask, commandThread);

					_commandThreads.push_back(commandThread);
				}
			}
			else
			{
				std::function<void()> task = _tasks.front();
				_tasks.pop();

				if (!commandThread->Open)
				{
					// reset the command allocator and command list.
					commandThread->CommandAllocator->Reset();
					ID3D12GraphicsCommandList* graphicsCmdList = static_cast<ID3D12GraphicsCommandList*>(commandThread->CommandList);
					graphicsCmdList->Reset(commandThread->CommandAllocator, nullptr);
					commandThread->Open = true;
				}

				_releaseMutex.lock();
				commandThread->Task = task;
				commandThread->Available = false;
				commandThread->Waiting = false;
				_releaseMutex.unlock();

				// Ensure the thread is ready to be notified.
				while (!commandThread->Waiting)
				{
					commandThread->WaitCondition.notify_one();
				}
			}
		}

		// Perform the remaining task on this thread.
		_commandAllocator->Reset();
		_commandList->Reset(_commandAllocator, nullptr);
		ResourceFactory::AssignCommandList(_commandList);
		std::function<void()> task = _tasks.front();
		_tasks.pop();
		task();
		_commandList->Close();
		commandLists.push_back(_commandList);

		// Wait for all tasks to complete.
		int complete = 0;
		while (complete != taskCount)
		{
			complete = 0;
			for (auto it = _commandThreads.begin(); it != _commandThreads.end(); ++it)
			{
				complete += (*it)->TasksCompleted;
			}
		}

		// Close command lists.
		for (auto it = _commandThreads.begin(); it != _commandThreads.end(); ++it)
		{
			CommandThread* commandThread = *it;
			if (commandThread->Open)
			{
				_releaseMutex.lock();
				commandThread->TasksCompleted = 0;
				commandThread->Available = true;
				commandThread->Open = false;

				ID3D12GraphicsCommandList* graphicsCmdList = static_cast<ID3D12GraphicsCommandList*>(commandThread->CommandList);
				graphicsCmdList->Close();
				commandLists.push_back(commandThread->CommandList);
				_releaseMutex.unlock();
			}
		}
#endif
		return commandLists;
	}
}

