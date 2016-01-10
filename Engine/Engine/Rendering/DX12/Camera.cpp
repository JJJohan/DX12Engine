#include "Camera.h"
#include "../../Factory/ResourceFactory.h"
#include "../../Input/Input.h"
#include "../../Core/Time.h"
#include "ConstantBuffer.h"
#include <sstream>

namespace Engine
{
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
		  , _pDevice(device)
		  , _pCbuffer(nullptr)
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

		_pCbuffer = ResourceFactory::CreateConstantBuffer();
		//_pCbuffer->GetResource()->SetName(L"Camera Data");
	}

	ConstantBuffer* Camera::GetCBuffer() const
	{
		return _pCbuffer;
	}

	Camera::~Camera()
	{
		if (_pCbuffer != nullptr)
		{
			delete _pCbuffer;
		}
	}

	void Camera::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		_pCbuffer->Bind(commandList);
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

			// Transpose matrices for shaders
			XMMATRIX viewT = XMMatrixTranspose(_view);
			XMMATRIX projT = XMMatrixTranspose(_projection);
			XMMATRIX vpT = XMMatrixTranspose(_vp);

			// Store data into buffer
			CameraData camData = {};
			XMStoreFloat4x4(&camData.view, viewT);
			XMStoreFloat4x4(&camData.proj, projT);
			XMStoreFloat4x4(&camData.vp, vpT);

			// Update the camera CBuffer.
			_pCbuffer->SetMatrix("view", camData.view);
			_pCbuffer->SetMatrix("proj", camData.proj);
			_pCbuffer->SetMatrix("vp", camData.vp);

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

	D3D12_VIEWPORT& Camera::GetViewPort()
	{
		return _viewport;
	}
}

