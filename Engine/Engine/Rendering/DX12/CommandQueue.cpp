#include <thread>
#include <sstream>
#include "CommandQueue.h"
#include "../../Utils/Logging.h"
#include "../../Factory/ResourceFactory.h"
#include "../../Utils/SystemInfo.h"


namespace Engine
{
	std::queue<std::function<void()>> CommandQueue::_tasks;
	std::mutex CommandQueue::_releaseMutex;
	bool CommandQueue::_releaseRequested = false;
	std::vector<CommandThread*> CommandQueue::_commandThreads;

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
			if (commandThread->Task == nullptr)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(1));
				if (commandThread->TasksCompleted == 0)
				{
					commandThread->IdleTimer += 0.001f;
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
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
				_releaseMutex.unlock();

				ID3D12GraphicsCommandList* graphicsCmdList = static_cast<ID3D12GraphicsCommandList*>(commandThread->CommandList);
				graphicsCmdList->Close();
				commandLists.push_back(commandThread->CommandList);
			}
		}

		// Cleanup idle threads.
		for (auto it = _commandThreads.begin(); it != _commandThreads.end(); ++it)
		{
			if ((*it)->IdleTimeOut)
			{
				std::stringstream ss;
				ss << " Reduced command threads from " << _commandThreads.size() << " to " << _commandThreads.size() - 1 << ".";
				Logging::Log(ss.str());

				CommandThread* commandThread = *it;
				commandThread->Thread.join();
				commandThread->CommandList->Release();
				commandThread->CommandAllocator->Release();
				delete commandThread;
				_commandThreads.erase(it);
				break; // One at a time is fine.
			}
		}

		return commandLists;
	}
}

