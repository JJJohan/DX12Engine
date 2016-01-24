using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX;

namespace EngineTests
{
	TEST_CLASS(StringTests)
	{
	public:
		TEST_METHOD(StringAssignment)
		{
			Engine::String string = "Hello";
			Assert::AreEqual("Hello", string.Str().c_str(), "String did not equal assignment of 'Hello'.");
			string = string + std::string(" World!");
			Assert::AreEqual("Hello World!", string.Str().c_str(), "String did not contain addition of ' World!'.");
			Assert::AreEqual('!', string[string.Length() - 1], L"String index of last item did not equal '!'.");

			int var = 5;
			Engine::String formatted = Engine::String::Format("var 1 = '{0}', var 2 = '{1}', var 1 + 3 = '{0}{2}'.", 1.2f, var, "test");
			Assert::AreEqual("var 1 = '1.2', var 2 = '5', var 1 + 3 = '1.2test'.", formatted.Str().c_str(), L"Formatted string was invalid.");
		}

		TEST_METHOD(StringMethods)
		{
			Engine::String string("    Hello World!   ");
			string = string.Trim();
			Assert::AreEqual("Hello World!", string.Str().c_str(), "String was not trimmed to 'Hello World!'.");
			string = string.Trim();
			Assert::AreEqual("Hello World!", string.Str().c_str(), "String did not remain unchanged from trimming again.");

			bool found = string.Contains("World");
			Assert::IsTrue(found, L"String could not find the word 'World'.");

			Assert::AreEqual("hello world!", string.ToLower().Str().c_str(), "String did not convert to lower case.");
			Assert::AreEqual("HELLO WORLD!", string.ToUpper().Str().c_str(), "String did not convert to upper case.");

			int i = 5;
			float f = 2.0f;
			double d = 1.0;
			Engine::String iString = i;
			Engine::String fString = f;
			Engine::String dString = d;
			Assert::AreEqual(i, iString.ToInt(), L"String did not equal input integer.");
			Assert::AreEqual(f, fString.ToFloat(), L"String did not equal input float.");
			Assert::AreEqual(d, dString.ToDouble(), L"String did not equal input double.");
		}
	};
}