#include "String.h"
#include "Engine/Data/String.h"
#include <msclr\marshal_cppstd.h>

namespace EngineNET
{
	String::String()
	{
		_pString = new Engine::String();
	}

	String::String(System::String^ string)
	{
		_pString = new Engine::String(Sys2Std(string));
	}

	String::String(std::string string)
	{
		_pString = new Engine::String(string);
	}

	String::~String()
	{
		this->!String();
	}

	String::!String()
	{
		delete _pString;
	}

	String^ String::operator+(System::String^ rhs) 
	{ 
		return gcnew String(Std2Sys(_pString->operator+(Sys2Std(rhs)).Str())); 
	}

	bool String::operator==(System::String^ rhs) 
	{ 
		return _pString->operator==(Sys2Std(rhs)); 
	}

	char String::operator[](int index)
	{
		return _pString->operator[](index);
	}

	String::operator System::String^()
	{
		return Std2Sys(_pString->Str());
	}

	String::operator Engine::String*()
	{
		return _pString;
	}

	std::string String::CStr()
	{
		return std::string(_pString->Str());
	}

	int String::Length()
	{
		return int(_pString->Length());
	}

	System::String^ String::Str()
	{
		return Std2Sys(_pString->Str());
	}

	std::string String::Sys2Std(System::String^ string)
	{
		msclr::interop::marshal_context context;
		return context.marshal_as<std::string>(string);
	}

	System::String^ String::Std2Sys(std::string string)
	{
		msclr::interop::marshal_context context;
		return context.marshal_as<System::String^>(string);
	}
}