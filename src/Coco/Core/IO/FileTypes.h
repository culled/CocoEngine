#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Types of file modes
	/// </summary>
	enum class FileModes
	{
		None = 0x00,
		Read = 0x01,
		Write = 0x02,
		Binary = 0x04,
	};

	COCOAPI FileModes operator&(FileModes a, FileModes b) noexcept;
	COCOAPI void operator&=(FileModes& a, FileModes b) noexcept;
	COCOAPI FileModes operator|(FileModes a, FileModes b) noexcept;
	COCOAPI void operator|=(FileModes& a, FileModes b) noexcept;
}