#pragma once

#include <Coco/Core/API.h>

#include <string>
#include <format>

namespace Coco
{
	/// @brief A generic string of characters
	using string = std::string;

	/// @brief Creates a string integrating a number of arguments. Ex: FormattedString("{0} {1}", "Hello", "Coco") would output "Hello Coco"
	/// @tparam ...Args 
	/// @param str The format for the string
	/// @param ...args The arguments that correspond to places in the format string
	/// @return A string
	template<typename ... Args>
	string FormattedString(const string str, Args&& ... args)
	{
		return std::vformat(str, std::make_format_args(std::forward<Args>(args)...));
	}

	/// @brief Returns a string with all leading and trailing whitespace removed
	/// @param str The string to trim
	/// @return The input string with leading and trailing whitespace removed
	COCOAPI string TrimWhitespace(const string& str);

	/// @brief Converts a basic type to a string
	/// @tparam T 
	/// @param value The value
	/// @return The value as a string
	template<typename T>
	COCOAPI string ToString(const T& value) { return std::to_string(value); }
}