#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// @brief Represents a path to a file
	class COCOAPI FilePath
	{
	public:
		const string Path;

	public:
		FilePath(const string& filePath);
		~FilePath() = default;

		/// @brief Gets the file name from a path
		/// @param path The file path
		/// @param withExtension If true, then the extension will be included in the file name
		/// @return The file name
		static string GetFileName(const string& path, bool withExtension = false);
	};
}