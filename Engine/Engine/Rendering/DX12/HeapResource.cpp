#include "HeapResource.h"
#include "HeapManager.h"
#include "CommandQueue.h"

namespace Engine
{
	HeapResource::HeapResource()
		: _pDevice(nullptr)
		, _pResource(nullptr)
		, _heapSize(0)
		, _dynamic(false)
		, _heapPending(false)
		, _lastHeapSize(0)
		, _pHeap(nullptr) { }

	HeapResource::~HeapResource()
	{
		HeapManager::ReleaseHeap(this);
	}

	bool HeapResource::PrepareHeapResource()
	{
		return PrepareHeapResource(CD3DX12_RESOURCE_DESC::Buffer(_heapSize));
	}

	bool HeapResource::PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc)
	{
		HeapDesc heapDesc;
		heapDesc.pHeapProperties = &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		heapDesc.HeapFlags = D3D12_HEAP_FLAG_NONE;
		heapDesc.InitialResourceState = D3D12_RESOURCE_STATE_COPY_DEST;
		heapDesc.pOptimizedClearValue = nullptr;

		return PrepareHeapResource(resourceDesc, heapDesc);
	}

	bool HeapResource::PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc, const HeapDesc& heapDesc)
	{
		bool newHeap = false;
		if (_pResource == nullptr || _lastHeapSize < _heapSize)
		{
			if (_pResource != nullptr && _lastHeapSize < _heapSize)
			{
				_pResource->Release();
			}

			_lastHeapSize = _heapSize;
			LOGFAILEDCOM(_pDevice->CreateCommittedResource(
				heapDesc.pHeapProperties,
				heapDesc.HeapFlags,
				&resourceDesc,
				heapDesc.InitialResourceState,
				heapDesc.pOptimizedClearValue,
				IID_PPV_ARGS(&_pResource)));

			newHeap = true;
		}

		return newHeap;
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
		_dynamic = true;
	}

	ID3D12Resource* HeapResource::GetResource() const
	{
		return _pResource;
	}
}

