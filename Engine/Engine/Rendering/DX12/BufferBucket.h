#pragma once

#include "BufferInstance.h"
#include "HeapResource.h"
#include <unordered_set>
#include <set>

struct ID3D12GraphicsCommandList;

namespace Engine
{
	class BufferInstance;

	class BufferBucket : public HeapResource
	{
	public:
		virtual ~BufferBucket();

		template <typename T>
		static T* PrepareBuffer(BufferInstance* instance);

	protected:
		BufferBucket();

		void ReleaseInstance(BufferInstance* instance);
		void RequestBuild();
		bool CheckBufferSize(size_t* outSize);
		void Unbind();
		bool Bound() const;

		virtual void Bind(ID3D12GraphicsCommandList* commandList) = 0;
		virtual void Build() = 0;

		std::set<BufferInstance*> _instances;
		bool _bound;
		int _rootSlot;

	private:
		static std::unordered_set<BufferBucket*> _buffers;

		size_t _totalSize;

		friend class BufferInstance;
	};

	template <typename T>
	T* BufferBucket::PrepareBuffer(BufferInstance* instance)
	{
		size_t instanceSize = instance->GetSize();

		for (auto it = _buffers.begin(); it != _buffers.end(); ++it)
		{
			BufferBucket* buffer = *it;
			if (dynamic_cast<T*>(buffer))
			{
				if (buffer->_rootSlot == instance->_rootSlot & buffer->_totalSize < 65536 - instanceSize)
				{
					buffer->_instances.insert(instance);
					buffer->_totalSize += instanceSize;
					buffer->RequestBuild();
					return static_cast<T*>(buffer);
				}
			}
		}

		T* buffer = new T();
		buffer->_pDevice = instance->_pDevice;
		buffer->_instances.insert(instance);
		buffer->_totalSize += instanceSize;
		buffer->_rootSlot = instance->_rootSlot;
		buffer->RequestBuild();
		_buffers.insert(buffer);
		return buffer;
	}
}

