#include <thread>
#include "CommandQueue.h"
#include "../../Utils/Logging.h"
#include "../../Factory/ResourceFactory.h"
#include "../../Utils/SystemInfo.h"

//#define SINGLE_THREADED

namespace Engine
{
	std::queue<std::function<void()>> CommandQueue::_tasks;
	std::mutex CommandQueue::_releaseMutex;
	bool CommandQueue::_releaseRequested = false;
	std::vector<CommandThread*> CommandQueue::_commandThreads;

#ifdef SINGLE_THREADED
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* commandList;
#endif

	void CommandQueue::Enqueue(const std::function<void()>& task)
	{
		_tasks.push(task);
	}

	void CommandQueue::Release()
	{
		_releaseMutex.lock();
		_releaseRequested = true;
		_releaseMutex.unlock();

		for (auto it = _commandThreads.begin(); it != _commandThreads.end(); ++it)
		{
			CommandThread* commandThread = *it;
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
			if (commandThread->Available)
			{
				if (commandThread->TasksCompleted == 0)
				{
					std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
					std::chrono::duration<float> diff = currentTime - commandThread->LastTime;
					commandThread->IdleTimer += diff.count();
					if (commandThread->IdleTimer > 2000.0f)
					{
						break;
					}
				}
			}
			else
			{
				commandThread->Task();

				commandThread->Mutex.lock();
				++commandThread->TasksCompleted;
				commandThread->Available = true;
				commandThread->Task = nullptr;
				commandThread->Mutex.unlock();
				commandThread->IdleTimer = 0.0f;
				commandThread->LastTime = std::chrono::high_resolution_clock::now();
			}
		}

		commandThread->Mutex.lock();
		commandThread->IdleTimeOut = true;
		commandThread->Mutex.unlock();
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
		if (commandList == nullptr)
		{
			LOGFAILEDCOM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
			LOGFAILEDCOM(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
		}
		else
		{
			commandAllocator->Reset();
			commandList->Reset(commandAllocator, nullptr);			
		}

		ResourceFactory::AssignCommandList(commandList);
		while (!_tasks.empty())
		{
			std::function<void()> task = _tasks.front();
			task();
			_tasks.pop();
		}
		commandList->Close();
		commandLists.push_back(commandList);
#else
		// Cleanup idle threads.
		bool idleFound = true;
		while (idleFound)
		{
			idleFound = false;
			for (auto it = _commandThreads.begin(); it != _commandThreads.end(); ++it)
			{
				if ((*it)->IdleTimeOut)
				{
					CommandThread* commandThread = *it;
					commandThread->Thread.join();
					commandThread->CommandList->Release();
					commandThread->CommandAllocator->Release();
					delete commandThread;
					idleFound = true;
					_commandThreads.erase(it);
					break;
				}
			}
		}

		// Begin work load.
		int taskCount = int(_tasks.size());
		while (!_tasks.empty())
		{
			CommandThread* commandThread = GetAvailableThread();
			if (commandThread == nullptr)
			{
				if (_commandThreads.size() < SystemInfo::GetCPUCores())
				{
					std::function<void()> task = _tasks.front();
					_tasks.pop();

					// Create the command list.
					commandThread = new CommandThread;
					commandThread->Available = false;
					commandThread->TasksCompleted = 0;
					commandThread->IdleTimeOut = false;
					commandThread->Task = task;
					commandThread->Open = true;

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
				_releaseMutex.unlock();
			}
		}

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

