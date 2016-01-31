#pragma once

namespace Engine
{
	struct FeatureInfo
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS D3D12Options;
		D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT VirtualAddressSupport;
		D3D12_FEATURE_DATA_FORMAT_SUPPORT FormatSupport;
		D3D12_FEATURE_DATA_FORMAT_INFO FormatInfo;
		D3D_FEATURE_LEVEL MaxFeatureLevelSupported;
		std::vector<int> MultiSampleLevels;
		BOOL UMASupported;
		BOOL CacheCoherentUMASupported;
		BOOL TileBasedRendererSupported;
	};

	struct AdapterInfo
	{
		std::string Name;
		size_t DedicatedMemory;
	};

	class FeatureSupport
	{
	public:
		static FeatureInfo QueryDeviceFeatures(ID3D12Device* device, DXGI_FORMAT format);
		static AdapterInfo QueryAdapterInfo(IDXGIAdapter1* adapter);
		static std::string GetFeatureLevelString(D3D_FEATURE_LEVEL featureLevel);
	};
}

