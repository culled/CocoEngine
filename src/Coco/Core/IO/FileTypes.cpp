#include "FileTypes.h"

namespace Coco
{
	FileModeFlags operator&(FileModeFlags a, FileModeFlags b) noexcept
	{
		return static_cast<FileModeFlags>(static_cast<int>(a) & static_cast<int>(b));
	}

	void operator&=(FileModeFlags& a, FileModeFlags b) noexcept
	{
		a = static_cast<FileModeFlags>(a & b);
	}

	FileModeFlags operator|(FileModeFlags a, FileModeFlags b) noexcept
	{
		return static_cast<FileModeFlags>(static_cast<int>(a) | static_cast<int>(b));
	}

	void operator|=(FileModeFlags& a, FileModeFlags b) noexcept
	{
		a = static_cast<FileModeFlags>(a | b);
	}

	bool operator>(FileModeFlags a, int b) noexcept
	{
		return static_cast<int>(a) > b;
	}
}