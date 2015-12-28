#include "Camera.h"
#include "../../Utils/Logging.h"
#include "d3dx12.h"
#include "CommandQueue.h"

namespace Engine
{
	Camera* Camera::CreateCamera(ID3D12Device* device, XMFLOAT4 screenRect, float fovInDegrees, float nearClip, float farClip)
	{
		Camera* camera = new Camera(device);
		camera->SetFOV(fovInDegrees);
		camera->_viewport.TopLeftX = screenRect.x;
		camera->_viewport.TopLeftY = screenRect.y;
		camera->_viewport.Width = screenRect.z - screenRect.x;
		camera->_viewport.Height = screenRect.w - screenRect.y;
		camera->_viewport.MinDepth = nearClip;
		camera->_viewport.MaxDepth = farClip;
		return camera;
	}

	Camera::Camera(ID3D12Device* device)
		: _fov(90.0f)
		, _transformed(true)
		, _pDevice(device)
	{
		// Create constant buffer view descriptor heap
		CD3DX12_RESOURCE_DESC constantBuffer = CD3DX12_RESOURCE_DESC::Buffer(sizeof XMFLOAT4X4 * 3);
		LOGFAILEDCOM(
			_pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&constantBuffer,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&_cbuffer)));

		LOGFAILEDCOM(
			_cbuffer->SetName(L"Camera Data"));
	}

	ID3D12Resource* Camera::GetCBuffer() const
	{
		return _cbuffer.Get();
	}

	Camera::~Camera()
	{
	}

	inline DirectX::XMVECTOR F2V(DirectX::XMFLOAT3 val)
	{
		return DirectX::XMLoadFloat3(&val);
	}

	inline DirectX::XMFLOAT3 V2F(DirectX::XMVECTOR vec)
	{
		DirectX::XMFLOAT3 val;
		XMStoreFloat3(&val, vec);
		return val;
	}

	bool Camera::Update()
	{
		bool transformed = _transformed;

		if (_transformed)
		{
			XMVECTOR up = _position + F2V(XMFLOAT3(0, 1, 0));
			XMVECTOR target = F2V(XMFLOAT3(0, 0, 0));
			_position = F2V(XMFLOAT3(0, 0, -5));
			_view = XMMatrixLookAtLH(_position, target, up);
			_projection = XMMatrixPerspectiveFovLH(_fov, _viewport.Width / _viewport.Height, 1.0f, 1000.0f);

			_vp = XMMatrixMultiply(_view, _projection);

			// Transpose matrices
			XMMATRIX viewT = XMMatrixTranspose(_view);
			XMMATRIX projT = XMMatrixTranspose(_projection);
			XMMATRIX vpT = XMMatrixTranspose(_vp);

			// Store as XMFLOAT4x4
			XMFLOAT4X4 view, proj, vp;
			XMStoreFloat4x4(&view, viewT);
			XMStoreFloat4x4(&proj, projT);
			XMStoreFloat4x4(&vp, vpT);

			int offset = sizeof XMFLOAT4X4;
			UINT8* memPtr;
			_cbuffer->Map(0, nullptr, reinterpret_cast<void**>(&memPtr));

			memcpy(memPtr, &view, sizeof offset);
			memcpy(memPtr + offset, &proj, sizeof offset);
			memcpy(memPtr + offset * 2, &vp, sizeof offset);

			_cbuffer->Unmap(0, nullptr);

			_transformed = false;
		}

		return transformed;
	}

	XMMATRIX Camera::GetViewMatrix() const
	{
		return _view;
	}

	XMMATRIX Camera::GetProjectionMatrix() const
	{
		return _projection;
	}

	XMMATRIX Camera::GetVPMatrix() const
	{
		return _vp;
	}

	XMVECTOR Camera::GetPosition() const
	{
		return _position;
	}

	void Camera::SetPosition(XMVECTOR position)
	{
		_position = position;
		_transformed = true;
	}

	XMVECTOR Camera::GetRotation() const
	{
		return _rotation;
	}

	void Camera::SetRotation(XMVECTOR rotation)
	{
		_rotation = rotation;
		_transformed = true;
	}

	float Camera::GetFOV() const
	{
		return XMConvertToDegrees(_fov);
	}

	void Camera::SetFOV(float fov)
	{
		_fov = XMConvertToRadians(fov);
		_transformed = true;
	}

	D3D12_VIEWPORT& Camera::GetViewPort()
	{
		return _viewport;
	}
}
