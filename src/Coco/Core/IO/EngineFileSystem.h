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
		/// @return The opened file
		virtual File OpenFile(const string& contentPath, FileOpenFlags openFlags) = 0;

		/// @brief Checks if a file exists in this file system
		/// @param contentPath The content path
		/// @return True if the file exists
		virtual bool FileExists(const string& contentPath) const = 0;

		/// @brief Creates a file in this file system if it doesn't already exist
		/// @param contentPath The content path
		/// @param openFlags The flags to open the file with
		/// @return The created file
		virtual File CreateFile(const string& contentPath, FileOpenFlags openFlags) = 0;
	};
}