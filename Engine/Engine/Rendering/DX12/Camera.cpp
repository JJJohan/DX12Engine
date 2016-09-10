#include "Camera.h"
#include "ConstantBuffer.h"
#include "DX12Renderer.h"

namespace Engine
{
	Camera* Camera::_pMainCamera = nullptr;

	void Camera::SetActiveCamera(Camera* camera)
	{
		if (_pMainCamera == camera)
		{
			return;
		}

		if (_pMainCamera != nullptr)
		{
			_pMainCamera->_pSceneBuffer->Unbind();
		}

		_pMainCamera = camera;
	}

	Camera* Camera::CreateCamera(ID3D12Device* device, float width, float height, float fovInDegrees, float nearClip, float farClip)
	{
		Camera* camera = new Camera(device);
		camera->_viewport.MinDepth = 0.0f;
		camera->_viewport.MaxDepth = 1.0f;
		camera->Resize(width, height);
		camera->SetFOV(fovInDegrees);
		camera->_nearClip = nearClip;
		camera->_farClip = farClip;

		camera->_pSceneBuffer->SetData("nearClip", nearClip);
		camera->_pSceneBuffer->SetData("farClip", farClip);

		return camera;
	}

	Camera* Camera::Main()
	{
		return _pMainCamera;
	}

	Camera::Camera(ID3D12Device* device)
		: _fov(90.0f)
		, _pDevice(device)
		, _nearClip(0.1f)
		, _farClip(100.0f)
		, _pSceneBuffer(nullptr)
	{
		Transform.SetPosition(0.0f, 0.0f, -1.0f);
		_pSceneBuffer = ResourceFactory::CreateConstantBuffer();
		_pSceneBuffer->SetRootSlot(2);
		_pSceneBuffer->AssignBuffer();

		Input::RegisterKey('W', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   this->Transform.Move(Vector3(0.0f, 0.0f, 1.0f * dt), Local);
		                   }, "__cameraForward");

		Input::RegisterKey('S', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   this->Transform.Move(Vector3(0.0f, 0.0f, -1.0f * dt), Local);
		                   }, "__cameraBackward");

		Input::RegisterKey('A', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   this->Transform.Move(Vector3(-1.0f * dt, 0.0f, 0.0f), Local);
		                   }, "__cameraLeft");

		Input::RegisterKey('D', KeyHeld, [&]
		                   {
			                   float dt = Time::DeltaTime();
			                   this->Transform.Move(Vector3(1.0f * dt, 0.0f, 0.0f), Local);
		                   }, "__cameraRight");

		Input::RegisterMouseMoveEvent([&](long x, long y)
		                              {
			                              this->Transform.Rotate(float(y) / 10, float(x) / 10, 0.0f);
		                              }, "__cameraRotate");
	}

	Camera::~Camera()
	{
		Input::UnregisterKey("__cameraForward");
		Input::UnregisterKey("__cameraBackward");
		Input::UnregisterKey("__cameraLeft");
		Input::UnregisterKey("__cameraRight");
		Input::UnregisterMouseMoveEvent("__cameraRotate");

		delete _pSceneBuffer;
	}

	void Camera::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		if (!_pSceneBuffer->Bound())
		{
			_pSceneBuffer->Bind(commandList);
		}
	}

	bool Camera::Update()
	{
		bool transformed = Transform.Moved;

		if (Transform.Moved)
		{
			// Get position and rotation.
			XMVECTOR rotation = Transform.GetRotation();
			XMVECTOR position = Transform.GetPosition();

			// Calculate local space directions.
			XMVECTOR forward = XMVector3Rotate(Vector3::Forward, rotation);
			XMVECTOR up = XMVector3Rotate(Vector3::Up, rotation);

			//update view matrix
			_view = XMMatrixLookToLH(position, forward, up);
			_vp = XMMatrixMultiply(_view, _projection);

			Transform.Moved = false;
		}

		return transformed;
	}

	void Camera::Resize(float width, float height)
	{
		_viewport.TopLeftX = 0.0f;
		_viewport.TopLeftY = 0.0f;
		_viewport.Width = width;
		_viewport.Height = height;
		SetFOV(-1.0f);
		Transform.Moved = true;
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
		if (fov > 0.0f)
		{
			_fov = XMConvertToRadians(fov);
		}
		
		_projection = XMMatrixPerspectiveFovLH(_fov, _viewport.Width / _viewport.Height, _farClip, _nearClip);
		Transform.Moved = true;
	}

	void Camera::ApplyTransform(ConstantBufferInstance* buffer, const Engine::Transform& transform) const
	{
		XMMATRIX mvp;
		if (!transform.GetIs2D())
		{
			mvp = XMMatrixTranspose(transform.GetMatrix() * _vp);
		}
		else
		{
			mvp = XMMatrixTranspose(transform.GetMatrix());
		}

		XMFLOAT4X4 mvpT;
		XMStoreFloat4x4(&mvpT, mvp);
		buffer->SetData("mvp", mvpT);
	}

	D3D12_VIEWPORT& Camera::GetViewPort()
	{
		return _viewport;
	}
}

