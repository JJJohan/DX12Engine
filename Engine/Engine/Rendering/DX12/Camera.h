#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Engine
{
	enum Space
	{
		World,
		Local
	};

	class Camera
	{
	public:
		static Camera* CreateCamera(ID3D12Device* device, const XMFLOAT4& screenRect, float fovInDegrees, float nearClip, float farClip);

		XMVECTOR GetPosition() const;
		void SetPosition(XMFLOAT3 position);
		void SetPosition(XMVECTOR position);

		XMFLOAT3 GetEulerAngles() const;
		void SetEulerAngles(float pitch, float yaw, float roll);
		void SetEulerAngles(const XMFLOAT3& eulerAngles);
		XMVECTOR GetRotation() const;
		void SetRotation(XMVECTOR rotation);

		void Move(XMFLOAT3 translation, Space relativeTo = World);
		void Rotate(const XMFLOAT3& eulerAngles);
		void Rotate(float pitch, float yaw, float roll);

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

		XMVECTOR _position;
		XMVECTOR _rotation;

		D3D12_VIEWPORT _viewport;
		float _fov;
		bool _transformed;
		ID3D12Device* _pDevice;
		ComPtr<ID3D12Resource> _cbuffer;

		XMMATRIX _view;
		XMMATRIX _projection;
		XMMATRIX _vp;

		static XMVECTOR _up;

		struct CameraData
		{
			XMFLOAT4X4 view, proj, vp;
		};
	};
}

