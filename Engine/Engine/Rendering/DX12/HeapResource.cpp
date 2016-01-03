#include "HeapResource.h"
#include "HeapManager.h"
#include "../../Utils/Logging.h"
#include "CommandQueue.h"
#include "d3dx12.h"

namespace Engine
{
	HeapResource::HeapResource()
		: _pResource(nullptr)
		, _heapSize(0)
		, _dynamic(false)
		, _heapPending(false)
		, _lastHeapSize(0)
		, _pHeap(nullptr)
	{
	}

	HeapResource::~HeapResource()
	{
		HeapManager::ReleaseHeap(this);
	}

	void HeapResource::PrepareHeapResource(size_t resourceSize)
	{
		if (_pResource == nullptr || _heapSize != resourceSize)
		{
			if (_pResource != nullptr && _heapSize != resourceSize)
			{
				_pResource->Release();
				_heapSize = resourceSize;
			}

			_heapSize = resourceSize;
			LOGFAILEDCOM(_pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(resourceSize),
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&_pResource)));
		}
	}

	void HeapResource::PrepareHeapResource(size_t resourceSize, const D3D12_RESOURCE_DESC& resourceDesc)
	{
		if (_pResource == nullptr || _heapSize != resourceSize)
		{
			if (_pResource != nullptr && _heapSize != resourceSize)
			{
				_pResource->Release();
				_heapSize = resourceSize;
			}

			_heapSize = resourceSize;
			LOGFAILEDCOM(_pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&_pResource)));
		}
	}

	void HeapResource::HeapTask(const std::function<void()>& heapTask)
	{
		if (!_heapPending)
		{
			CommandQueue::Enqueue(heapTask);
			_heapPending = true;
		}
	}

	void HeapResource::MarkDynamic()
	{
		if (_pHeap != nullptr)
		{
			Logging::LogError("Tried to mark a heap dynamic when it was already in use.");
			return;
		}

		_dynamic = true;
	}
}
