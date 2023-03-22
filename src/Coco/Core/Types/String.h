#pragma once

#include <Coco/Core/API.h>

#include <string>
#include <format>

namespace Coco
{
	/// <summary>
	/// A generic string of characters
	/// </summary>
	using string = std::string;

	/// <summary>
	/// Creates a string integrating a number of arguments.
	/// Ex: FormattedString("{0} {1}", "Hello", "Coco") would output "Hello Coco"
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="str">The format for the string</param>
	/// <param name="...args">The arguments that correspond to places in the format string</param>
	/// <returns>A formatted string</returns>
	template<typename ... Args>
	string FormattedString(const string str, Args&& ... args)
	{
		return std::vformat(str, std::make_format_args(std::forward<Args>(args)...));
	}

	/// <summary>
	/// Returns a string with all leading and trailing whitespace removed
	/// </summary>
	/// <param name="str">The string to trim</param>
	/// <returns>The input string with leading and trailing whitespace removed</returns>
	COCOAPI string TrimWhitespace(const string& str);

	template<typename T>
	COCOAPI string ToString(const T& value) { return std::to_string(value); }
}