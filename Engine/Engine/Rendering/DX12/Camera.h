#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include "../../Data/Transform.h"

using namespace DirectX;

namespace Engine
{
	class ConstantBuffer;

	class Camera
	{
	public:
		static Camera* CreateCamera(ID3D12Device* device, const XMFLOAT4& screenRect, float fovInDegrees, float nearClip, float farClip);
		static Camera* Main();

		~Camera();

		float GetFOV() const;
		void SetFOV(float fovInDegrees);

		D3D12_VIEWPORT& GetViewPort();
		XMMATRIX GetViewMatrix() const;
		XMMATRIX GetProjectionMatrix() const;
		XMMATRIX GetVPMatrix() const;

		bool Update();
		void ApplyTransform(ConstantBuffer* buffer, const Transform& transform) const;
		void Resize(const XMFLOAT4& screenRect, float fovInDegrees, float nearClip, float farClip);

		Transform Transform;

	private:
		Camera(ID3D12Device* device);

		static Camera* _pMainCamera;

		D3D12_VIEWPORT _viewport;
		float _fov;
		ID3D12Device* _pDevice;
		bool _resized;

		XMMATRIX _view;
		XMMATRIX _projection;
		XMMATRIX _vp;

		struct CameraData
		{
			XMFLOAT4X4 view, proj, vp;
		};
	};
}

