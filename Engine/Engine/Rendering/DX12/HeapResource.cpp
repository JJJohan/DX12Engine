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
		, _customHeapDesc(false)
		, _lastHeapSize(0)
		, _pHeap(nullptr)
	{ }

	HeapResource::~HeapResource()
	{
		HeapManager::ReleaseHeap(this);
	}

	bool HeapResource::PrepareHeapResource(D3D12_RESOURCE_STATES state)
	{
		return PrepareHeapResource(CD3DX12_RESOURCE_DESC::Buffer(_heapSize));
	}

	bool HeapResource::PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_RESOURCE_STATES state)
	{
		HeapDesc heapDesc;
		if (_customHeapDesc)
		{
			heapDesc = _heapDesc;
		}
		else
		{
			heapDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
			heapDesc.HeapFlags = D3D12_HEAP_FLAG_NONE;
			heapDesc.InitialResourceState = state;
			heapDesc.pOptimizedClearValue = nullptr;
		}

		return PrepareHeapResource(resourceDesc, heapDesc);
	}

	bool HeapResource::PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc, HeapDesc& heapDesc)
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
				&CD3DX12_HEAP_PROPERTIES(heapDesc.HeapType),
				heapDesc.HeapFlags,
				&resourceDesc,
				heapDesc.InitialResourceState,
				heapDesc.pOptimizedClearValue,
				IID_PPV_ARGS(&_pResource)));

			_resourceState = heapDesc.InitialResourceState;

			newHeap = true;
		}

		return newHeap;
	}

	void HeapResource::SetHeapDescription(D3D12_HEAP_TYPE heapType, D3D12_HEAP_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE* clearValue)
	{
		_customHeapDesc = true;
		_heapDesc.HeapType = heapType;
		_heapDesc.HeapFlags = flags;
		_heapDesc.InitialResourceState = initialState;
		_heapDesc.pOptimizedClearValue = clearValue;
	}

	void HeapResource::HeapTask(const std::function<void()>& heapTask)
	{
		CommandQueue::Enqueue(heapTask);
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

