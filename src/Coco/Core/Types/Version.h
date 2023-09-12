#pragma once

#include "../Defines.h"

namespace Coco
{
	/// @brief Represents a version with major, minor, and patch numbers
	struct Version
	{
		/// @brief The major version
		uint32 Major;

		/// @brief The minor version
		uint32 Minor;

		/// @brief The patch version
		uint32 Patch;

		Version();
		Version(uint32 major, uint32 minor, uint32 patch);
	};
}