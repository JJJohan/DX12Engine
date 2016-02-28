#pragma once

using namespace DirectX;

namespace Engine
{
	class D3DUtils
	{
	public:
		static void Initialise(ID3D12Device* device);
		static bool Succeeded(ID3DBlob* blob);
		
		static UINT GetSRVDescriptorSize();
		static UINT GetRTVDescriptorSize();
		static UINT GetDSVDescriptorSize();

	private:
		static ID3D12Device* _pDevice;

		static UINT _cbvSrvDescriptorSize;
		static UINT _rtvDescriptorSize;
		static UINT _dsvDescriptorSize;
	};

	
}
