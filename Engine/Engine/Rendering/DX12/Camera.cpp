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
		camera->Resize(screenRect, fovInDegrees, nearClip, farClip);

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
		, _resized(false)
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
	}

	Camera::~Camera()
	{
		
	}

	bool Camera::Update()
	{
		bool transformed = Transform.Moved;

		// Calculate projection matrix
		if (_resized)
		{
			_projection = XMMatrixPerspectiveFovLH(_fov, _viewport.Width / _viewport.Height, _viewport.MinDepth, _viewport.MaxDepth);
			_resized = false;
		}

		if (Transform.Moved)
		{
			Transform.SetRotation(XMQuaternionNormalize(Transform.GetRotation()));
			XMVECTOR direction = XMVector3Rotate(Vector3::Forward, Transform.GetRotation());

			// Calculate view matrix
			_view = XMMatrixLookToLH(Transform.GetPosition(), direction, Vector3::Up);
			_vp = XMMatrixMultiply(_view, _projection);

			Transform.Moved = false;
		}

		return transformed;
	}

	void Camera::Resize(const XMFLOAT4& screenRect, float fovInDegrees, float nearClip, float farClip)
	{
		SetFOV(fovInDegrees);
		_viewport.TopLeftX = screenRect.x;
		_viewport.TopLeftY = screenRect.y;
		_viewport.Width = screenRect.z - screenRect.x;
		_viewport.Height = screenRect.w - screenRect.y;
		_viewport.MinDepth = nearClip;
		_viewport.MaxDepth = farClip;
		_resized = true;
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

	void Camera::ApplyTransform(ConstantBufferInstance* buffer, const Engine::Transform& transform) const
	{
		XMMATRIX mvp = XMMatrixTranspose(transform.GetMatrix() * _vp);
		XMFLOAT4X4 mvpT;
		XMStoreFloat4x4(&mvpT, mvp);

		buffer->SetMatrix("mvp", mvpT);
	}

	D3D12_VIEWPORT& Camera::GetViewPort()
	{
		return _viewport;
	}
}

