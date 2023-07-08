#pragma once

#include <Coco/Core/API.h>

#include "String.h"

namespace Coco
{
	/// @brief Represents a version with a major, minor, and patch number
	struct COCOAPI Version
	{
		/// @brief The major version
		int Major = 0;

		/// @brief The minor version
		int Minor = 0;

		/// @brief The patch version
		int Patch = 0;

		Version() = default;
		Version(int major, int minor, int patch) noexcept;
		virtual ~Version() = default;

		/// @brief Gets a string representation of this version
		/// @return The string representation of this version
		string ToString() const { return FormattedString("{}.{}.{}", Major, Minor, Patch); }
	};
}