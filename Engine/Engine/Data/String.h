#pragma once

#include <string>
#include <vector>

#ifndef ENGINENET_EXPORTS
#include "../External/src/CPPFormat/format.h"
#endif

namespace Engine
{
	class String
	{
	public:
		ENGINE_API String();
		ENGINE_API String(std::stringstream stream);
		ENGINE_API String(const char* string);
		ENGINE_API String(const wchar_t* string);
		ENGINE_API String(std::string string);
		ENGINE_API String(std::wstring string);
		ENGINE_API String(int value);
		ENGINE_API String(float value);
		ENGINE_API String(double value);

		ENGINE_API String operator+(const String& rhs) const;
		ENGINE_API bool operator==(const String& rhs) const;
		ENGINE_API char operator[](size_t index);

		ENGINE_API operator std::string() const;
		ENGINE_API operator std::wstring() const;
		ENGINE_API operator std::stringstream() const;

		ENGINE_API int ToInt() const;
		ENGINE_API float ToFloat() const;
		ENGINE_API double ToDouble() const;

		ENGINE_API String Trim();
		ENGINE_API String ToLower() const;
		ENGINE_API String ToUpper() const;
		ENGINE_API bool Contains(String string) const;
		ENGINE_API size_t Length() const;
		ENGINE_API std::vector<String> Split(char delim) const;
		ENGINE_API const std::string& Str() const;

#ifndef ENGINENET_EXPORTS
		template <typename... Args>
		static String Format(const String& string, Args ... args)
		{
			std::string text = fmt::format(string._string, args...);
			return String(text);
		}
#else
		static String Format(const String& string)
		{
			return string;
		}
#endif

	private:
		std::string _string;

		std::vector<String>& Split(char delim, std::vector<String>& elems) const;
	};
}

