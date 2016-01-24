#pragma once

#include "../stdafx.h"
#include <string>

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
		std::string CStr();

		int Length();
		System::String^ Str();

	private:
		Engine::String* _pString;

		std::string Sys2Std(System::String^ string);
		System::String^ Std2Sys(std::string string);
	};
}