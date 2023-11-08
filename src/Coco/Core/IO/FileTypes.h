#pragma once
#include "../Corepch.h"
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

	/// @brief A path to a file or directory
	class FilePath
	{
		friend class File;

	private:
		std::filesystem::path _filePath;

	public:
		FilePath();
		FilePath(const char* filePath);
		FilePath(const string& filePath);
		FilePath(const std::filesystem::path& filePath);

		bool operator==(const FilePath& other) const;
		bool operator!=(const FilePath& other) const { return !(*this == other); }
		FilePath operator/(const FilePath& b) const;
		void operator/=(const FilePath& b);

		/// @brief Gets a relative path to a path
		/// @param path The path
		/// @param relativeTo The path to be relative to
		/// @return A relative path, or the original path if no relative path could be found
		static FilePath GetRelativePath(const FilePath& path, const FilePath& relativeTo);

		/// @brief Combines multiple individual elements into a path
		/// @tparam ...Args The types of arguments
		/// @param base The base path
		/// @param first The first part to add to the base path
		/// @param ...others The other parts to add to the base path
		/// @return The combined path
		template<typename ... Args>
		static FilePath CombineToPath(const FilePath& base, const string& first, Args&& ... others)
		{
			FilePath combined(base._filePath / first);
			return CombineToPath(combined, others...);
		}

		/// @brief Gets the current working directory for the application
		/// @return The current working directory path
		static FilePath GetCurrentWorkingDirectoryPath();

		/// @brief Gets the name of the file for this path
		/// @param includeExtension If true, the extension will be included as part of the file name
		/// @return The file name
		string GetFileName(bool includeExtension) const;

		/// @brief Gets the extension of this file, including the "."
		/// @return The file extension
		string GetExtension() const;

		/// @brief Gets the path of the directory that contains this file/directory
		/// @return The parent directory path
		FilePath GetParentDirectory() const;

		/// @brief Determines if this file path represents a relative file path
		/// @return True if this file path represents a relative file path
		bool IsRelative() const;

		/// @brief Determines if this path points to a directory
		/// @return True if this path points to a directory
		bool IsDirectory() const;

		/// @brief Determines if this path is empty
		/// @return True if this path is empty
		bool IsEmpty() const;

		/// @brief Gets this path as a string
		/// @return This path as a string
		string ToString() const;

		operator std::filesystem::path() const { return _filePath; }

	private:
		/// @brief Overloads the CombineToPath for a single path
		/// @param base The base path
		/// @return The base path
		static FilePath CombineToPath(const FilePath& base)
		{
			return base;
		}
	};
}