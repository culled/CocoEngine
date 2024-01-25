#pragma once

#include "../Types/String.h"

namespace Coco
{
	/// @brief Flags to open a file with
	enum class FileOpenFlags
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Text = 1 << 2
	};

	constexpr FileOpenFlags operator|(const FileOpenFlags& a, const FileOpenFlags& b)
	{
		return static_cast<FileOpenFlags>(static_cast<int>(a) | static_cast<int>(b));
	}

	constexpr FileOpenFlags operator&(const FileOpenFlags& a, const FileOpenFlags& b)
	{
		return static_cast<FileOpenFlags>(static_cast<int>(a) & static_cast<int>(b));
	}

	constexpr void operator|=(FileOpenFlags& a, const FileOpenFlags& b)
	{
		a = a | b;
	}

	constexpr void operator&=(FileOpenFlags& a, const FileOpenFlags& b)
	{
		a = a & b;
	}

	/// @brief Gets the string representation of an iostate
	/// @param state The state
	/// @return The string representation
	const char* IOStateToString(std::ios_base::iostate state);

	/// @brief Gets the string representation of a flags for opening a file
	/// @param flags The flags
	/// @return The string representation
	string FileOpenFlagsToString(FileOpenFlags flags);
}