#pragma once

#include "Resource.h"

namespace Coco
{
	class COCOAPI FileResource : public Resource
	{
	protected:
		string _filePath;
		bool _isLoaded;

	public:
		FileResource(ResourceID id, const string& name, uint64_t tickLifetime, const string& filePath) noexcept;

		virtual ~FileResource() = default;

		/// @brief Sets this resource's file path
		/// @param filePath The file path
		void SetFilePath(const string& filePath) { _filePath = filePath; }

		/// @brief Gets the path to the file this resource is loaded from (if any)
		/// @return The path to the file this resource is loaded from, or an empty string if this resource isn't saved to disk
		const string& GetFilePath() const noexcept { return _filePath; }

		/// @brief Gets if this resource has a file path. This usually means it relates to a file on the disk
		/// @return True if this resource has a file path
		bool HasFilePath() const noexcept { return !_filePath.empty(); }

		/// @brief Gets if this resource has been fully loaded
		/// @return True if this resource has been fully loaded
		bool IsLoaded() const noexcept { return _isLoaded; }
	};
}