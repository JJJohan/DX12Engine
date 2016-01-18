#include "HeapManager.h"
#include "d3dx12.h"
#include "../../Utils/Logging.h"
#include "../../Factory/ResourceFactory.h"
#include "HeapResource.h"

namespace Engine
{
	std::vector<HeapResource*> HeapManager::_dynamicUploadHeaps = std::vector<HeapResource*>();
	std::vector<HeapResource*> HeapManager::_staticUploadHeaps = std::vector<HeapResource*>();
	ID3D12Device* HeapManager::_pDevice = nullptr;
	std::mutex HeapManager::_mutex;

	void HeapManager::RequestHeap(HeapResource* heapResource, bool addToList)
	{
		// Create the GPU upload buffer.
		LOGFAILEDCOM(_pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(heapResource->_heapSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&heapResource->_pHeap)));

		if (addToList)
		{
			_mutex.lock();
			if (!heapResource->_dynamic)
			{
				_staticUploadHeaps.push_back(heapResource);
			}
			else
			{
				_dynamicUploadHeaps.push_back(heapResource);
			}
			_mutex.unlock();
		}
	}

	void HeapManager::ReleaseHeap(HeapResource* heapResource)
	{
		if (heapResource->_pHeap != nullptr)
		{
			heapResource->_pHeap->Release();
			heapResource->_pHeap = nullptr;
		}

		if (heapResource->_pResource != nullptr)
		{
			heapResource->_pResource->Release();
			heapResource->_pResource = nullptr;
		}
	}

	void HeapManager::UpdateHeaps()
	{
		_mutex.lock();
		for (auto it = _staticUploadHeaps.begin(); it != _staticUploadHeaps.end(); ++it)
		{
			HeapResource* heapResource = *it;
			heapResource->_heapSize = 0;
			heapResource->_lastHeapSize = 0;
			heapResource->_heapPending = false;
			if (heapResource->_pHeap != nullptr)
			{
				heapResource->_pHeap->Release();
				heapResource->_pHeap = nullptr;
			}
		}
		_staticUploadHeaps.clear();
		_mutex.unlock();

		for (auto it = _dynamicUploadHeaps.begin(); it != _dynamicUploadHeaps.end(); ++it)
		{
			(*it)->_heapPending = false;
		}
	}

	void HeapManager::ReleaseHeaps()
	{
		for (auto it = _staticUploadHeaps.begin(); it != _staticUploadHeaps.end(); ++it)
		{
			HeapResource* heapResource = *it;
			if (heapResource->_pHeap != nullptr)
			{
				heapResource->_pHeap->Release();
				heapResource->_pHeap = nullptr;
			}
		}
		_staticUploadHeaps.clear();
	}

	void HeapManager::Upload(HeapResource* heap, void* data, enum D3D12_RESOURCE_STATES destState)
	{
		Upload(heap, data, int(heap->_heapSize), int(heap->_heapSize), destState);
	}

	void HeapManager::Upload(HeapResource* heap, void* data, int rowPitch, int slicePitch, enum D3D12_RESOURCE_STATES destState)
	{
		if (heap->_pHeap == nullptr)
		{
			if (heap->_heapSize == 0)
			{
				Logging::LogError("A heap resource upload request is being made without a heap size being specified.");
				return;
			}

			RequestHeap(heap, true);
		}

		ID3D12GraphicsCommandList* commandList = static_cast<ID3D12GraphicsCommandList*>(ResourceFactory::GetCommandList());

		if (!heap->_dynamic)
		{
			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = data;
			subresourceData.RowPitch = LONG_PTR(rowPitch);
			subresourceData.SlicePitch = LONG_PTR(slicePitch);

			UpdateSubresources(commandList, heap->_pResource, heap->_pHeap, 0, 0, 1, &subresourceData);

			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(heap->_pResource, D3D12_RESOURCE_STATE_COPY_DEST, destState));
		}
		else
		{
			UINT8* dest;
			heap->_pResource = heap->_pHeap;
			CD3DX12_RANGE readRange(0, 0);
			LOGFAILEDCOM(heap->_pResource->Map(0, &readRange, reinterpret_cast<void**>(&dest)));
			memcpy(dest, data, rowPitch);
			heap->_pHeap->Unmap(0, nullptr);
		}
	}

	void HeapManager::SetDevice(ID3D12Device* device)
	{
		_pDevice = device;
	}
}

