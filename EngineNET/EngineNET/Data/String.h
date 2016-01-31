#pragma once

namespace Engine
{
	class String;
}

namespace EngineNET
{
	public ref class String
	{
	public:
		String();
		String(System::String^ string);
		String(std::string string);
		~String();
		!String();

		String^ operator+(System::String^ rhs);
		bool operator==(System::String^ rhs);
		char operator[](int index);
		operator System::String^();
		operator Engine::String*();
		operator std::string();

		int Length();
		System::String^ Str();

		static std::string Sys2Std(System::String^ string);
		static System::String^ Std2Sys(std::string string);

	private:
		Engine::String* _pString;
	};
}