namespace Engine
{
	ID3D12Device* D3DUtils::_pDevice = nullptr;
	UINT D3DUtils::_cbvSrvDescriptorSize = 0;
	UINT D3DUtils::_rtvDescriptorSize = 0;
	UINT D3DUtils::_dsvDescriptorSize = 0;

	void D3DUtils::Initialise(ID3D12Device* device)
	{
		_pDevice = device;
		_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	bool D3DUtils::Succeeded(ID3DBlob* blob)
	{
		if (blob != nullptr)
		{
			size_t size = blob->GetBufferSize();
			char* buf = new char[size];
			memcpy_s(buf, size, blob->GetBufferPointer(), size);
			Logging::LogError(buf);
			delete[] buf;
			blob->Release();

			return false;
		}

		return true;
	}

	UINT D3DUtils::GetSRVDescriptorSize()
	{
		return _cbvSrvDescriptorSize;
	}

	UINT D3DUtils::GetRTVDescriptorSize()
	{
		return _rtvDescriptorSize;
	}

	UINT D3DUtils::GetDSVDescriptorSize()
	{
		return _dsvDescriptorSize;
	}
}