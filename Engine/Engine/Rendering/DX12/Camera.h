#pragma once

using namespace DirectX;

struct D3D12_VIEWPORT;
struct ID3D12Device;

namespace Engine
{
	class ConstantBufferInstance;
	class ConstantBuffer;

	class Camera
	{
	public:
		static Camera* CreateCamera(ID3D12Device* device, float width, float height, float fovInDegrees, float nearClip, float farClip);
		static ENGINE_API Camera* Main();

		~Camera();

		ENGINE_API float GetFOV() const;
		ENGINE_API void SetFOV(float fovInDegrees);

		D3D12_VIEWPORT& GetViewPort();
		XMMATRIX GetViewMatrix() const;
		XMMATRIX GetProjectionMatrix() const;
		XMMATRIX GetVPMatrix() const;

		bool Update();
		void ApplyTransform(ConstantBufferInstance* buffer, const Transform& transform) const;
		void Resize(float width, float height);
		Transform Transform;

	private:
		Camera(ID3D12Device* device);

		static Camera* _pMainCamera;

		D3D12_VIEWPORT _viewport;
		float _fov;
		ID3D12Device* _pDevice;

		XMMATRIX _view;
		XMMATRIX _projection;
		XMMATRIX _vp;

		struct CameraData
		{
			XMFLOAT4X4 view, proj, vp;
		};
	};
}

