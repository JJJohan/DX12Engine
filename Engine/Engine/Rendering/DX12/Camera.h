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

		float GetFOV() const;
		void SetFOV(float fovInDegrees);

		D3D12_VIEWPORT& GetViewPort();
		XMMATRIX GetViewMatrix() const;
		XMMATRIX GetProjectionMatrix() const;
		XMMATRIX GetVPMatrix() const;
		ConstantBuffer* GetCBuffer() const;

		bool Update();
		void Bind(ID3D12GraphicsCommandList* commandList) const;

		~Camera();

		Transform Transform;

	private:
		Camera(ID3D12Device* device);

		D3D12_VIEWPORT _viewport;
		float _fov;
		ID3D12Device* _pDevice;
		ConstantBuffer* _pCbuffer;

		XMMATRIX _view;
		XMMATRIX _projection;
		XMMATRIX _vp;

		struct CameraData
		{
			XMFLOAT4X4 view, proj, vp;
		};
	};
}

