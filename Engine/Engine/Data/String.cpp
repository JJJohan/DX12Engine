#include <sstream>
#include <vector>
#include "String.h"

namespace Engine
{
	String::String()
	{
	}

	String::String(std::stringstream stream)
	{
		_string = stream.str();
	}

	String::String(const char* string)
	{
		_string = string;
	}

	String::String(std::string string)
	{
		_string = string;
	}

	String::String(int value)
	{
		std::stringstream ss;
		ss << value;
		_string = ss.str();
	}

	String::String(float value)
	{
		std::stringstream ss;
		ss << value;
		_string = ss.str();
	}

	String::String(double value)
	{
		std::stringstream ss;
		ss << value;
		_string = ss.str();
	}

	String::String(const wchar_t* string)
	{
		std::wstring w = std::wstring(string);
		_string = std::string(w.begin(), w.end());
	}

	String::String(std::wstring string)
	{
		_string = std::string(string.begin(), string.end());
	}

	String String::operator+(const String& rhs) const
	{
		return _string + rhs._string;
	}

	bool String::operator==(const String& rhs) const
	{
		return _string == rhs._string;
	}

	char String::operator[](size_t index)
	{
		return _string[index];
	}

	size_t String::Length() const
	{
		return _string.length();
	}

	String String::Trim()
	{
		size_t start = 0;
		size_t end = _string.length();

		while (start < end && (_string[start] == ' ' || _string[start] == '\t'))
		{
			++start;
		}

		while (end > start && (_string[end] == ' ' || _string[end] == '\t'))
		{
			--end;
		}

		return _string.substr(start, end - start);
	}

	bool String::Contains(String string) const
	{
		return _string.find(string._string) != std::string::npos;
	}

	String String::ToLower() const
	{
		std::string s = _string;

		size_t size = _string.size();
		for (size_t i = 0; i < size; ++i)
		{
			s[i] = tolower(s[i]);
		}

		return s;
	}

	String String::ToUpper() const
	{
		std::string s = _string;

		size_t size = _string.size();
		for (size_t i = 0; i < size; ++i)
		{
			s[i] = toupper(s[i]);
		}

		return s;
	}

	std::vector<String>& String::Split(char delim, std::vector<String>& elems) const
	{
		std::stringstream ss(_string);
		std::string item;
		while (getline(ss, item, delim))
		{
			if (!item.empty())
			{
				elems.push_back(item);
			}
		}
		return elems;
	}

	std::vector<String> String::Split(char delim) const
	{
		std::vector<String> elems;
		Split(delim, elems);
		return elems;
	}

	String::operator std::string() const
	{
		return _string;
	}

	String::operator std::wstring() const
	{
		return std::wstring(_string.begin(), _string.end());
	}

	int String::ToInt() const
	{
		return stoi(_string);
	}

	float String::ToFloat() const
	{
		return stof(_string);
	}

	double String::ToDouble() const
	{
		return stod(_string);
	}

	String::operator std::stringstream() const
	{
		return std::stringstream(_string);
	}

	const std::string& String::Str() const
	{
		return _string;
	}
}

