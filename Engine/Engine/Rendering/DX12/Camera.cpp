#include "Camera.h"
#include "../../Input/Input.h"
#include "../../Core/Time.h"
#include "ConstantBuffer.h"
#include <sstream>

namespace Engine
{
	Camera* Camera::_pMainCamera = nullptr;

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

		_pMainCamera = camera;

		return camera;
	}

	Camera* Camera::Main()
	{
		return _pMainCamera;
	}

	Camera::Camera(ID3D12Device* device)
		: _fov(90.0f)
		  , _pDevice(device)
	{
		Transform.SetPosition(0.0f, 0.0f, -1.0f);

		Input::RegisterKey('W', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   this->Transform.Move(Vector3(0.0f, 0.0f, 1.0f * dt), Local);
		                   });

		Input::RegisterKey('S', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   this->Transform.Move(Vector3(0.0f, 0.0f, -1.0f * dt), Local);
		                   });

		Input::RegisterKey('A', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   this->Transform.Move(Vector3(-1.0f * dt, 0.0f, 0.0f), Local);
		                   });

		Input::RegisterKey('D', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   this->Transform.Move(Vector3(1.0f * dt, 0.0f, 0.0f), Local);
		                   });

		Input::RegisterMouseMoveEvent([&](long x, long y)
			{
				this->Transform.Rotate(float(y) / 10, float(x) / 10, 0.0f);
			});

		//_pCbuffer->GetResource()->SetName(L"Camera Data");
	}

	bool Camera::Update()
	{
		bool transformed = Transform.Moved;

		if (Transform.Moved)
		{
			Transform.SetRotation(XMQuaternionNormalize(Transform.GetRotation()));
			XMVECTOR direction = XMVector3Rotate(Vector3::Forward, Transform.GetRotation());

			// Calculate view matrix
			_view = XMMatrixLookToLH(Transform.GetPosition(), direction, Vector3::Up);

			// Calculate projection matrix
			_projection = XMMatrixPerspectiveFovLH(_fov, _viewport.Width / _viewport.Height, 0.01f, 100.0f);

			// Calculate view projection matrix
			_vp = XMMatrixMultiply(_view, _projection);

			Transform.Moved = false;
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

	float Camera::GetFOV() const
	{
		return XMConvertToDegrees(_fov);
	}

	void Camera::SetFOV(float fov)
	{
		_fov = XMConvertToRadians(fov);
		Transform.Moved = true;
	}

	void Camera::ApplyWorldMatrix(ConstantBuffer* constantBuffer, const XMMATRIX& worldMatrix) const
	{
		XMMATRIX mvp = XMMatrixTranspose(worldMatrix * _vp);
		XMFLOAT4X4 mvpT;
		XMStoreFloat4x4(&mvpT, mvp);

		constantBuffer->SetMatrix("mvp", mvpT);
	}

	D3D12_VIEWPORT& Camera::GetViewPort()
	{
		return _viewport;
	}
}

