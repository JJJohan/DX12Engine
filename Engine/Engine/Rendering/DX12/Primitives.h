#pragma once

namespace Engine
{
	class RenderObject;

	class Primitives
	{
	public:
		ENGINE_API static RenderObject* CreateCube(Vector3 pos = Vector3::Zero, Quaternion rot = Quaternion::Identity, Vector3 scale = Vector3::One);
		ENGINE_API static RenderObject* CreateCube(std::string name, Vector3 pos = Vector3::Zero, Quaternion rot = Quaternion::Identity, Vector3 scale = Vector3::One);

		ENGINE_API static RenderObject* CreateSphere(int samples, Vector3 pos = Vector3::Zero, Quaternion rot = Quaternion::Identity, Vector3 scale = Vector3::One);
		ENGINE_API static RenderObject* CreateSphere(std::string name, int samples, Vector3 pos = Vector3::Zero, Quaternion rot = Quaternion::Identity, Vector3 scale = Vector3::One);

		ENGINE_API static RenderObject* CreateCylinder(int sides, Vector3 pos = Vector3::Zero, Quaternion rot = Quaternion::Identity, Vector3 scale = Vector3::One);
		ENGINE_API static RenderObject* CreateCylinder(std::string name, int sides, Vector3 pos = Vector3::Zero, Quaternion rot = Quaternion::Identity, Vector3 scale = Vector3::One);

		ENGINE_API static RenderObject* CreateQuad(Vector3 pos = Vector3::Zero, Quaternion rot = Quaternion::Identity, Vector3 scale = Vector3::One);
		ENGINE_API static RenderObject* CreateQuad(std::string name, Vector3 pos = Vector3::Zero, Quaternion rot = Quaternion::Identity, Vector3 scale = Vector3::One);

	private:
		Primitives() {}
	};
}

