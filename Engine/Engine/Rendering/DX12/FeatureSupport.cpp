#include "FeatureSupport.h"
#include "../../Utils/Logging.h"
#include <vector>


namespace Engine
{
	AdapterInfo FeatureSupport::QueryAdapterInfo(IDXGIAdapter1* adapter)
	{
		AdapterInfo adapterInfo;

		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		std::wstring wideDesc = std::wstring(desc.Description);
		adapterInfo.DedicatedMemory = desc.DedicatedVideoMemory / 1024 / 1024;
		adapterInfo.Name = std::string(wideDesc.begin(), wideDesc.end());
		
		return adapterInfo;
	}

	std::string FeatureSupport::GetFeatureLevelString(D3D_FEATURE_LEVEL featureLevel)
	{
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_12_1:
			return "DirectX 12.1";
		case D3D_FEATURE_LEVEL_12_0:
			return "DirectX 12";
		case D3D_FEATURE_LEVEL_11_1:
			return "DirectX 11.1";
		case D3D_FEATURE_LEVEL_11_0:
			return "DirectX 11";
		case D3D_FEATURE_LEVEL_10_1:
			return "DirectX 10.1";
		case D3D_FEATURE_LEVEL_10_0:
			return "DirectX 10";
		case D3D_FEATURE_LEVEL_9_3:
			return "DirectX 9.3c";
		case D3D_FEATURE_LEVEL_9_2:
			return "DirectX 9.2";
		case D3D_FEATURE_LEVEL_9_1:
			return "DirectX 9.1";
		default:
			return "DirectX 12.1+";
		}
	}

	FeatureInfo FeatureSupport::QueryDeviceFeatures(ID3D12Device* device, DXGI_FORMAT format)
	{
		FeatureInfo featureInfo;
		
		// Query D3D12 Options
		LOGFAILEDRETURN(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &featureInfo.D3D12Options, sizeof(featureInfo.D3D12Options)),
			"Failed querying for D3D12_FEATURE_D3D12_OPTIONS.", featureInfo);

		// Query Architecture Support
		D3D12_FEATURE_DATA_ARCHITECTURE architecture;
		architecture.NodeIndex = 0;
		LOGFAILEDRETURN(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &architecture, sizeof(architecture)),
			"Failed querying for D3D12_FEATURE_ARCHITECTURE.", featureInfo);
		featureInfo.CacheCoherentUMASupported = architecture.CacheCoherentUMA;
		featureInfo.UMASupported = architecture.UMA;
		featureInfo.TileBasedRendererSupported = architecture.TileBasedRenderer;

		// Query Feature Levels
		D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels;
		featureLevels.NumFeatureLevels = 9;
		D3D_FEATURE_LEVEL* levels = new D3D_FEATURE_LEVEL[featureLevels.NumFeatureLevels] 
		{ D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, 
			D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };
		featureLevels.pFeatureLevelsRequested = levels;
		LOGFAILEDRETURN(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels)),
			"Failed querying for D3D12_FEATURE_FEATURE_LEVELS.", featureInfo);
		delete[] levels;
		featureInfo.MaxFeatureLevelSupported = featureLevels.MaxSupportedFeatureLevel;

		// Query Multisample Quality Levels
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msLevels;
		msLevels.Format = format;
		int sampleLevels = 2;
		while (true)
		{
			msLevels.SampleCount = sampleLevels;
			LOGFAILEDRETURN(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msLevels, sizeof(msLevels)),
				"Failed querying for D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS.", featureInfo);

			if (msLevels.NumQualityLevels > 0)
			{
				featureInfo.MultiSampleLevels.push_back(sampleLevels);
				sampleLevels *= 2;
			}
			else
			{
				break;
			}
		}

		// Query Format Support
		featureInfo.FormatSupport.Format = format;
		LOGFAILEDRETURN(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &featureInfo.FormatSupport, sizeof(featureInfo.FormatSupport)),
			"Failed querying for D3D12_FEATURE_FORMAT_SUPPORT.", featureInfo);

		// Query Format Info
		featureInfo.FormatInfo.Format = format;
		LOGFAILEDRETURN(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_INFO, &featureInfo.FormatInfo, sizeof(featureInfo.FormatInfo)),
			"Failed querying for D3D12_FEATURE_FORMAT_INFO.", featureInfo);

		// Query GPU Virtual Address Support
		LOGFAILEDRETURN(device->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &featureInfo.VirtualAddressSupport, sizeof(featureInfo.VirtualAddressSupport)),
			"Failed querying for D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT.", featureInfo);

		return featureInfo;
	}
}
