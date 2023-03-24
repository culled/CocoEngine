#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// @brief Flags for modes of opening files
	enum class FileModeFlags
	{
		/// @brief An invalid file mode
		None = 0,

		/// @brief The file can be read from
		Read = 1 << 0,

		/// @brief The file can be written to
		Write = 1 << 1,

		/// @brief The file will be interpreted as binary data
		Binary = 1 << 2,
	};

	COCOAPI FileModeFlags operator&(FileModeFlags a, FileModeFlags b) noexcept;
	COCOAPI void operator&=(FileModeFlags& a, FileModeFlags b) noexcept;
	COCOAPI FileModeFlags operator|(FileModeFlags a, FileModeFlags b) noexcept;
	COCOAPI void operator|=(FileModeFlags& a, FileModeFlags b) noexcept;
}