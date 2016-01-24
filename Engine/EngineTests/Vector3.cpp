using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX;

namespace EngineTests
{		
	TEST_CLASS(Vector3Tests)
	{
	public:	
		XMVECTOR xmvec = XMVectorSet(1.0f, 2.0f, 3.0f, 4.0f);
		XMFLOAT3 xmfloat3 = XMFLOAT3(3.0f, 2.0f, 1.0f);
		Engine::Vector3 vec1 = Engine::Vector3(5.2f, 3.6f, -84.1f);
		Engine::Vector3 vec2 = xmvec;
		Engine::Vector3 vec3 = xmfloat3;

		TEST_METHOD(Vector3Assignment)
		{
			// Test direct assignment
			Assert::AreEqual(5.2f, vec1.X, 0.01f, L"vec1 X did not equal 5.2f.");
			Assert::AreEqual(3.6f, vec1.Y, 0.01f, L"vec1 Y did not equal 3.6f.");
			Assert::AreEqual(-84.1f, vec1.Z, 0.01f, L"vec1 Z did not equal -84.1f.");
		
			// Test XMVECTOR assignment
			Assert::AreEqual(xmvec.m128_f32[0], vec2.X, 0.01f, L"vec2 X did not equal xmvec X.");
			Assert::AreEqual(xmvec.m128_f32[1], vec2.Y, 0.01f, L"vec2 Y did not equal xmvec Y.");
			Assert::AreEqual(xmvec.m128_f32[2], vec2.Z, 0.01f, L"vec2 Z did not equal xmvec Z.");
			
			// Test XMFLOAT3 assignment
			Assert::AreEqual(xmfloat3.x, vec3.X, 0.01f, L"vec3 X did not equal xmfloat3 X.");
			Assert::AreEqual(xmfloat3.y, vec3.Y, 0.01f, L"vec3 Y did not equal xmfloat3 Y.");
			Assert::AreEqual(xmfloat3.z, vec3.Z, 0.01f, L"vec3 Z did not equal xmfloat3 Z.");

			// Test implicit conversion to XMFLOAT3
			XMFLOAT3 xmfloat = vec2;
			Assert::AreEqual(xmfloat.x, vec2.X, 0.01f, L"xmfloat3 X did not equal vec2 X.");
			Assert::AreEqual(xmfloat.y, vec2.Y, 0.01f, L"xmfloat3 Y did not equal vec2 Y.");
			Assert::AreEqual(xmfloat.z, vec2.Z, 0.01f, L"xmfloat3 Z did not equal vec2 Z.");

			// Test implicit conversion to XMVECTOR
			XMVECTOR xmv = vec3;
			Assert::AreEqual(xmv.m128_f32[0], vec3.X, 0.01f, L"xmvec X did not equal vec3 X.");
			Assert::AreEqual(xmv.m128_f32[1], vec3.Y, 0.01f, L"xmvec Y did not equal vec3 Y.");
			Assert::AreEqual(xmv.m128_f32[2], vec3.Z, 0.01f, L"xmvec Z did not equal vec3 Z.");
		}

		TEST_METHOD(Vector3Operations)
		{
			XMFLOAT3 add = XMFLOAT3(5.0f, 5.0f, 5.0f);
			Engine::Vector3 mul = Engine::Vector3(2.0f, 2.0f, 2.0f);

			// Test multiplication with XMVECTOR
			vec2 *= mul;
			Assert::AreEqual(xmvec.m128_f32[0] * mul.X, vec2.X, 0.01f, L"vec2 X did not equal expected value.");
			Assert::AreEqual(xmvec.m128_f32[1] * mul.Y, vec2.Y, 0.01f, L"vec2 Y did not equal expected value.");
			Assert::AreEqual(xmvec.m128_f32[2] * mul.Z, vec2.Z, 0.01f, L"vec2 Z did not equal expected value.");

			// Test addition with XMFLOAT3
			vec3 += add;
			Assert::AreEqual(xmfloat3.x + add.x, vec3.X, 0.01f, L"vec3 X did not equal expected value.");
			Assert::AreEqual(xmfloat3.y + add.y, vec3.Y, 0.01f, L"vec3 Y did not equal expected value.");
			Assert::AreEqual(xmfloat3.z + add.z, vec3.Z, 0.01f, L"vec3 Z did not equal expected value.");
		}
	};
}