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

	protected:
		HeapResource();
		virtual ~HeapResource();

		void PrepareHeapResource();
		void PrepareHeapResource(const D3D12_RESOURCE_DESC& resourceDesc);
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

