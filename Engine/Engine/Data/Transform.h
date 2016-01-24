#pragma once

#include "Vector3.h"
#include "Quaternion.h"
#include <functional>

namespace Engine
{
	enum Space
	{
		Local,
		World
	};

	class Transform
	{
	public:
		ENGINE_API Transform();

		ENGINE_API void SetPosition(float x, float y, float z);
		ENGINE_API void SetPosition(const Vector3& position);
		ENGINE_API void SetRotation(const Quaternion& rotation);
		ENGINE_API void SetScale(float x, float y, float z);
		ENGINE_API void SetScale(const Vector3& scale);

		ENGINE_API void Move(const Vector3& translation, Space relativeTo = World);
		ENGINE_API void Rotate(const Vector3& eulerAngles);
		ENGINE_API void Rotate(float pitch, float yaw, float roll);
		ENGINE_API void LookAt(float x, float y, float z);
		ENGINE_API void LookAt(const Vector3& position);

		ENGINE_API Vector3 GetPosition() const;
		ENGINE_API Quaternion GetRotation() const;
		ENGINE_API Vector3 GetScale() const;
		ENGINE_API const DirectX::XMMATRIX& GetMatrix() const;
		ENGINE_API Vector3 GetEulerAngles() const;

		void SetPosOverride(std::function<Vector3(float, float, float)> func);
		void SetRotOverride(std::function<Quaternion(float, float, float, float)> func);
		void SetScaleOverride(std::function<Vector3(float, float, float)> func);

		void GetPosOverride(std::function<Vector3(float, float, float)> func);
		void GetRotOverride(std::function<Quaternion(float, float, float, float)> func);
		void GetScaleOverride(std::function<Vector3(float, float, float)> func);

		bool Moved;

	private:
		void UpdateMatrix();

		Vector3 _position;
		Quaternion _rotation;
		Vector3 _eulerAngles;
		Vector3 _scale;

		std::function<Vector3(float, float, float)> _setPosOverrideFunc;
		std::function<Quaternion(float, float, float, float)> _setRotOverrideFunc;
		std::function<Vector3(float, float, float)> _setScaleOverrideFunc;

		std::function<Vector3(float, float, float)> _getPosOverrideFunc;
		std::function<Quaternion(float, float, float, float)> _getRotOverrideFunc;
		std::function<Vector3(float, float, float)> _getScaleOverrideFunc;

		bool _setPosOverride;
		bool _setRotOverride;
		bool _setScaleOverride;

		bool _getPosOverride;
		bool _getRotOverride;
		bool _getScaleOverride;

		DirectX::XMMATRIX _worldMatrix;
	};
}

