using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX;

namespace EngineTests
{
	TEST_CLASS(TransformTests)
	{
	public:
		TEST_METHOD(TransformMethods)
		{
			Engine::Transform transform;

			transform.SetPosition(Engine::Vector3(1.0f, 2.0f, 3.0f));
			Engine::Vector3 pos = transform.GetPosition();
			Assert::AreEqual(1.0f, pos.X, 0.01f, L"Pos X did not equal input of 1.0f.");
			Assert::AreEqual(2.0f, pos.Y, 0.01f, L"Pos Y did not equal input of 2.0f.");
			Assert::AreEqual(3.0f, pos.Z, 0.01f, L"Pos Z did not equal input of 3.0f.");

			transform.SetRotation(Engine::Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
			Engine::Quaternion rot = transform.GetRotation();
			Assert::AreEqual(1.0f, rot.X, 0.01f, L"Rot X did not equal input of 1.0f.");
			Assert::AreEqual(2.0f, rot.Y, 0.01f, L"Rot Y did not equal input of 2.0f.");
			Assert::AreEqual(3.0f, rot.Z, 0.01f, L"Rot Z did not equal input of 3.0f.");
			Assert::AreEqual(4.0f, rot.W, 0.01f, L"Rot W did not equal input of 4.0f.");

			transform.SetScale(Engine::Vector3(1.0f, 2.0f, 3.0f));
			Engine::Vector3 scale = transform.GetScale();
			Assert::AreEqual(1.0f, scale.X, 0.01f, L"Scale X did not equal input of 1.0f.");
			Assert::AreEqual(2.0f, scale.Y, 0.01f, L"Scale Y did not equal input of 2.0f.");
			Assert::AreEqual(3.0f, scale.Z, 0.01f, L"Scale Z did not equal input of 3.0f.");
		}
	};
}