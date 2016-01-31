#include "BufferBucket.h"

namespace Engine
{
	std::unordered_set<BufferBucket*> BufferBucket::_buffers;

	BufferBucket::BufferBucket()
		: _bound(false)
		  , _totalSize(0)
	{
	}

	BufferBucket::~BufferBucket()
	{
		auto it = _buffers.find(this);
		if (it != _buffers.end())
		{
			_buffers.erase(it);
		}
	}

	void BufferBucket::ReleaseInstance(BufferInstance* instance)
	{
		if (_instances.empty())
		{
			Logging::LogWarning("Tried to delete a buffer instance while the buffer is already empty.");
			return;
		}

		auto it = _instances.find(instance);
		if (it != _instances.end())
		{
			_instances.erase(it);
		}

		if (_instances.empty())
		{
			delete this;
		}
	}

	bool BufferBucket::CheckBufferSize(size_t* outSize)
	{
		// Calculate total size of buffer.
		size_t size = 0;
		for (auto it = _instances.begin(); it != _instances.end(); ++it)
		{
			BufferInstance* instance = *it;
			size += instance->GetSize();
		}

		if (size == 0)
		{
			Logging::LogError("Buffer is empty.");
		}

		if (size >= 65536)
		{
			Logging::LogError("Buffer exceeded maximum size of 64KB.");
			return false;
		}

		*outSize = size;
		return true;
	}

	void BufferBucket::RequestBuild()
	{
		HeapTask(std::bind(&BufferBucket::Build, this));
	}

	void BufferBucket::Unbind()
	{
		_bound = false;
	}
}

