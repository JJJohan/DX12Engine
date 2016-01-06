#pragma once

#include <string>
#include <vector>
#include "../External/src/CPPFormat/format.h"

namespace Engine
{
	class String
	{
	public:
		String();
		String(std::stringstream stream);
		String(const char* string);
		String(const wchar_t* string);
		String(std::string string);
		String(std::wstring string);
		String(int value);
		String(float value);
		String(double value);

		String operator+(const String& rhs) const;
		bool operator==(const String& rhs) const;
		char operator[](size_t index);

		operator std::string() const;
		operator std::wstring() const;
		operator std::stringstream() const;

		String Trim();
		String ToLower() const;
		String ToUpper() const;
		bool Contains(String string) const;
		size_t Length() const;
		std::vector<String> Split(const String& s, char delim) const;

		template <typename... Args>
		static String Format(const String& string, Args... args)
		{
			std::string text = fmt::format(string._string, args...);
			return String(text);
		}

	private:
		std::string _string;

		std::vector<String>& Split(const String& s, char delim, std::vector<String>& elems) const;
	};
}
