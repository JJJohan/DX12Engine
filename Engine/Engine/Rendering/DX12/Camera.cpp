#include "Camera.h"
#include "../../Utils/Logging.h"
#include "d3dx12.h"
#include "../../Input/Input.h"
#include "../../Core/Time.h"
#include <sstream>

namespace Engine
{
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

	XMVECTOR Camera::_up = F2V(XMFLOAT3(0.0f, 1.0f, 0.0f));

	Camera* Camera::CreateCamera(ID3D12Device* device, const XMFLOAT4& screenRect, float fovInDegrees, float nearClip, float farClip)
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

		_position = F2V(XMFLOAT3(0, 0, -1));
		_rotation = XMQuaternionIdentity();

		Input::RegisterKey('W', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   Move(XMFLOAT3(0.0f, 0.0f, 1.0f * dt), Local);
		                   });

		Input::RegisterKey('S', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   Move(XMFLOAT3(0.0f, 0.0f, -1.0f * dt), Local);
		                   });

		Input::RegisterKey('A', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   Move(XMFLOAT3(-1.0f * dt, 0.0f, 0.0f), Local);
		                   });

		Input::RegisterKey('D', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   Move(XMFLOAT3(1.0f * dt, 0.0f, 0.0f), Local);
		                   });

		Input::RegisterMouseMoveEvent([&](long x, long y)
			{
				Rotate(float(y) / 10, float(x) / 10, 0.0f);
			});

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

	bool Camera::Update()
	{
		bool transformed = _transformed;

		if (_transformed)
		{
			XMVECTOR forward = F2V(XMFLOAT3(0, 0, 1));

			_rotation = XMQuaternionNormalize(_rotation);
			XMVECTOR direction = XMVector3Rotate(forward, _rotation);

			// Calculate view matrix
			_view = XMMatrixLookToLH(_position, direction, _up);

			// Calculate projection matrix
			_projection = XMMatrixPerspectiveFovLH(_fov, _viewport.Width / _viewport.Height, 0.01f, 100.0f);

			// Calculate view projection matrix
			_vp = XMMatrixMultiply(_view, _projection);

			// Transpose matrices for shaders
			XMMATRIX viewT = XMMatrixTranspose(_view);
			XMMATRIX projT = XMMatrixTranspose(_projection);
			XMMATRIX vpT = XMMatrixTranspose(_vp);

			// Store data into buffer
			CameraData camData = {};
			XMStoreFloat4x4(&camData.view, viewT);
			XMStoreFloat4x4(&camData.proj, projT);
			XMStoreFloat4x4(&camData.vp, vpT);

			// Map data to cbuffer
			UINT8* memPtr;
			_cbuffer->Map(0, nullptr, reinterpret_cast<void**>(&memPtr));
			memcpy(memPtr, &camData, sizeof(camData));
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

	void Camera::SetPosition(XMFLOAT3 position)
	{
		SetPosition(F2V(position));
	}

	XMVECTOR Camera::GetRotation() const
	{
		return _rotation;
	}

	XMFLOAT3 Camera::GetEulerAngles() const
	{
		XMFLOAT4 q;
		XMStoreFloat4(&q, _rotation);

		XMFLOAT3 angles;
		angles.x = atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * (q.y * q.y) - 2 * (q.z * q.z));
		angles.y = asin(2 * q.x * q.y + 2 * q.z * q.w);
		angles.z = atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * (q.x * q.x) - 2 * (q.z * q.z));

		return angles;
	}

	void Camera::SetEulerAngles(const XMFLOAT3& eulerAngles)
	{
		float pitch = XMConvertToRadians(eulerAngles.x);
		float yaw = XMConvertToRadians(eulerAngles.y);
		float roll = XMConvertToRadians(eulerAngles.z);

		_rotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
		_transformed = true;
	}

	void Camera::SetEulerAngles(float pitch, float yaw, float roll)
	{
		SetEulerAngles(XMFLOAT3(pitch, yaw, roll));
	}

	void Camera::SetRotation(XMVECTOR rotation)
	{
		_rotation = rotation;
		_transformed = true;
	}

	void Camera::Move(XMFLOAT3 translation, Space relativeTo)
	{
		XMVECTOR trans = F2V(translation);
		if (relativeTo == Local)
		{
			trans = XMVector3Rotate(trans, _rotation);
		}

		_position = _position + trans;
		_transformed = true;
	}

	void Camera::Rotate(const XMFLOAT3& eulerAngles)
	{
		float pitch = XMConvertToRadians(eulerAngles.x);
		float yaw = XMConvertToRadians(eulerAngles.y);
		float roll = XMConvertToRadians(eulerAngles.z);

		XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
		_rotation = XMQuaternionMultiply(_rotation, rotation);

		_transformed = true;
	}

	void Camera::Rotate(float pitch, float yaw, float roll)
	{
		Rotate(XMFLOAT3(pitch, yaw, roll));
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

