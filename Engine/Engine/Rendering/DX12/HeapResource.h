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

		struct HeapDesc
		{
			D3D12_HEAP_PROPERTIES *pHeapProperties;
			D3D12_HEAP_FLAGS HeapFlags;
			D3D12_RESOURCE_STATES InitialResourceState;
			D3D12_CLEAR_VALUE *pOptimizedClearValue;
		};

	protected:
		HeapResource();
		virtual ~HeapResource();

		bool PrepareHeapResource();
		bool PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc);
		bool PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc, const HeapDesc& heapDesc);
		void HeapTask(const std::function<void()>& heapTask);
		void MarkDynamic();

		ID3D12Device* _pDevice;
		ID3D12Resource* _pResource;
		unsigned long long _heapSize;

	private:
		bool _dynamic;
		bool _heapPending;
		unsigned long long _lastHeapSize;
		ID3D12Resource* _pHeap;

		friend class HeapManager;
	};
}

