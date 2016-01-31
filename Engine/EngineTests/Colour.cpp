using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX;

namespace EngineTests
{
	TEST_CLASS(ColourTests)
	{
	public:
		TEST_METHOD(ColourClamping)
		{
			Engine::Colour col;
			col = col + col.White;
			Assert::AreEqual(1.0f, col.GetRed(), 0.01f, L"Colour R value is not 1.0f.");
			Assert::AreEqual(1.0f, col.GetGreen(), 0.01f, L"Colour G value is not 1.0f.");
			Assert::AreEqual(1.0f, col.GetBlue(), 0.01f, L"Colour B value is not 1.0f.");
			Assert::AreEqual(1.0f, col.GetAlpha(), 0.01f, L"Colour A value is not 1.0f.");

			col = Engine::Colour::White;
			col = col - Engine::Colour(10.0f, 10.0f, 10.0f);
			col = col + Engine::Colour(0.1f, 0.1f, 0.1f);
			Assert::AreEqual(0.1f, col.GetRed(), 0.01f, L"Colour R value is not 0.1f.");
			Assert::AreEqual(0.1f, col.GetGreen(), 0.01f, L"Colour G value is not 0.1f.");
			Assert::AreEqual(0.1f, col.GetBlue(), 0.01f, L"Colour B value is not 0.1f.");
			Assert::AreEqual(1.0f, col.GetAlpha(), 0.01f, L"Colour A value is not 1.0f.");

			Engine::Colour col2 = Engine::Colour(-3.5f, 5.0f, .0123f);
			Assert::AreEqual(0.0f, col2.GetRed(), 0.01f, L"Colour R value is not 0.0f.");
			Assert::AreEqual(1.0f, col2.GetGreen(), 0.01f, L"Colour G value is not 1.0f.");
			Assert::AreEqual(0.0123f, col2.GetBlue(), 0.01f, L"Colour B value is not 0.0123f.");
			Assert::AreEqual(1.0f, col2.GetAlpha(), 0.01f, L"Colour A value is not 1.0f.");
		}
	};
}

