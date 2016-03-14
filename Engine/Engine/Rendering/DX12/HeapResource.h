#pragma once

#include <functional>

struct D3D12_RESOURCE_DESC;
struct ID3D12Resource;
struct ID3D12Device;

namespace Engine
{
	class HeapResource
	{
	public:
		ID3D12Resource* GetResource() const;
		D3D12_RESOURCE_STATES GetResourceState() const;
		
		struct HeapDesc
		{
			D3D12_HEAP_TYPE HeapType;
			D3D12_HEAP_FLAGS HeapFlags;
			D3D12_RESOURCE_STATES InitialResourceState;
			D3D12_CLEAR_VALUE* pOptimizedClearValue;
		};

		void SetHeapDescription(D3D12_HEAP_TYPE heapType, D3D12_HEAP_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE* clearValue);

	protected:
		HeapResource();
		virtual ~HeapResource();

		bool PrepareHeapResource();
		bool PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc);
		bool PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc, HeapDesc& heapDesc);
		void HeapTask(const std::function<void()>& heapTask);
		void MarkDynamic();

		ID3D12Device* _pDevice;
		ID3D12Resource* _pResource;
		unsigned long long _heapSize;

	private:
		bool _dynamic;
		bool _heapPending;
		bool _customHeapDesc;
		unsigned long long _lastHeapSize;
		ID3D12Resource* _pHeap;
		HeapDesc _heapDesc;
		D3D12_RESOURCE_STATES _resourceState;

		friend class HeapManager;
	};
}

