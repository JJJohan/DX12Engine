#pragma once

#include "ConstantBuffer.h"

using namespace DirectX;

struct D3D12_VIEWPORT;
struct ID3D12Device;

namespace Engine
{
	class ConstantBufferInstance;
	class ConstantBuffer;
	class Matrix;

	class Camera
	{
	public:
		static void SetActiveCamera(Camera* camera);
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
		void Bind(ID3D12GraphicsCommandList* commandList) const;
		Transform Transform;

		template <class T>
		void SetSceneBufferData(std::string name, T value)
		{
			ID3D12GraphicsCommandList* commandList = static_cast<ID3D12GraphicsCommandList*>(ResourceFactory::GetCommandList());
			if (_pSceneBuffer->Bound())
			{
				_pSceneBuffer->Unbind();
			}

			_pSceneBuffer->SetData(name, value);
			_pSceneBuffer->Bind(commandList, 2);
		}

	private:
		Camera(ID3D12Device* device);

		static Camera* _pMainCamera;

		D3D12_VIEWPORT _viewport;
		float _fov;
		ID3D12Device* _pDevice;

		XMMATRIX _view;
		XMMATRIX _projection;
		XMMATRIX _vp;
		float _nearClip;
		float _farClip;
		ConstantBufferInstance* _pSceneBuffer;

		struct CameraData
		{
			XMFLOAT4X4 view, proj, vp;
		};
	};
}

