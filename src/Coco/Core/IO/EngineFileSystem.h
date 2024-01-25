#pragma once

#include "File.h"

namespace Coco
{
	/// @brief A file system that an Engine uses
	class EngineFileSystem
	{
	public:
		virtual ~EngineFileSystem() = default;

		/// @brief Opens a file at the given content path
		/// @param contentPath The content path
		/// @param openFlags The flags to open the file with
		/// @param createIfNonexistent If true, the file will be created if it doesn't exist
		/// @return The opened file
		virtual File OpenFile(const FilePath& contentPath, FileOpenFlags openFlags, bool createIfNonexistent = false) = 0;

		/// @brief Checks if a file exists in this file system
		/// @param contentPath The content path
		/// @return True if the file exists
		virtual bool FileExists(const FilePath& contentPath) const = 0;

		/// @brief Converts the given path to a full path, if applicable
		/// @param contentPath The content path
		/// @return The full path
		virtual FilePath ConvertToFullPath(const FilePath& contentPath) const = 0;

		/// @brief Converts the given path to a short path, if applicable
		/// @param contentPath The content path
		/// @return The shortened path
		virtual FilePath ConvertToShortPath(const FilePath& contentPath) const = 0;

		/// @brief Determines if two file paths point to the same file
		/// @param filePath1 The first file path
		/// @param filePath2 The secont file path
		/// @return True if both paths point to the same file
		virtual bool AreSameFile(const FilePath& filePath1, const FilePath& filePath2) const = 0;

		/// @brief Creates a file in this file system if it doesn't already exist
		/// @param contentPath The content path
		/// @param openFlags The flags to open the file with
		/// @return The created file
		virtual File CreateFile(const FilePath& contentPath, FileOpenFlags openFlags) = 0;
	};
}