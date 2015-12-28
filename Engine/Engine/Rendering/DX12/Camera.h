#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Engine
{
	class Camera
	{
	public:
		static Camera* CreateCamera(ID3D12Device* device, XMFLOAT4 screenRect, float fovInDegrees, float nearClip, float farClip);

		XMVECTOR GetPosition() const;
		void SetPosition(XMVECTOR position);

		XMVECTOR GetRotation() const;
		void SetRotation(XMVECTOR rotation);

		float GetFOV() const;
		void SetFOV(float fovInDegrees);

		D3D12_VIEWPORT& GetViewPort();
		XMMATRIX GetViewMatrix() const;
		XMMATRIX GetProjectionMatrix() const;
		XMMATRIX GetVPMatrix() const;
		ID3D12Resource* GetCBuffer() const;

		bool Update();

		~Camera();
	private:
		Camera(ID3D12Device* device);
		
		D3D12_VIEWPORT _viewport;
		XMVECTOR _position;
		XMVECTOR _rotation;
		float _fov;
		bool _transformed;
		ID3D12Device* _pDevice;
		ComPtr<ID3D12Resource> _cbuffer;
		XMMATRIX _view;
		XMMATRIX _projection;
		XMMATRIX _vp;
	};
}