#pragma once

#include "../Corepch.h"
#include "../Defines.h"

namespace Coco
{
	/// @brief A wrapper for std::string
	using string = std::string;

	/// @brief A wrapper for std::wstring
	using wstring = std::wstring;

	/// @brief Converts a string to a wide string
	/// @param str The string
	wstring StringToWideString(const char* str);

	/// @brief Converts a wide string to a string
	/// @param wStr The wide string
	string WideStringToString(const wchar_t* wStr);

	/// @brief Formats a string from the given format string and arguments
	/// @param format The format string
	/// @param args The arguments to insert into the formatted string
	/// @return The formatted string
	template<typename ... Args>
	string FormatString(const char* format, Args&& ... args)
	{
		return std::vformat(format, std::make_format_args(args...));
	}
}