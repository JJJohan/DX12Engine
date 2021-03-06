#pragma once

namespace Engine
{
	class HeapResource;

	class HeapManager
	{
	public:
		static void RequestHeap(HeapResource* heapResource, bool addToList);
		static void UpdateHeaps();
		static void ReleaseHeaps();
		static void ReleaseHeap(HeapResource* heapResource);
		static void Upload(HeapResource* heap, void* data);
		static void Upload(HeapResource* heap, void* data, int rowPitch, int slicePitch);
		static void SetDevice(ID3D12Device* device);

	private:
		static std::vector<HeapResource*> _staticUploadHeaps;
		static std::vector<HeapResource*> _dynamicUploadHeaps;
		static ID3D12Device* _pDevice;
		static std::mutex _mutex;
	};
}

