#include "BufferBucket.h"
#include "BufferInstance.h"
#include "../../Utils/Logging.h"

namespace Engine
{
	std::unordered_set<BufferBucket*> BufferBucket::_buffers;

	BufferBucket::BufferBucket()
		: _totalSize(0)
		, _bound(false)
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

