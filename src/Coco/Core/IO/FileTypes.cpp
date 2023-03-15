#include "FileTypes.h"

namespace Coco
{
	FileModes operator&(FileModes a, FileModes b) noexcept
	{
		return static_cast<FileModes>(static_cast<int>(a) & static_cast<int>(b));
	}

	void operator&=(FileModes& a, FileModes b) noexcept
	{
		a = static_cast<FileModes>(a & b);
	}

	FileModes operator|(FileModes a, FileModes b) noexcept
	{
		return static_cast<FileModes>(static_cast<int>(a) | static_cast<int>(b));
	}

	void operator|=(FileModes& a, FileModes b) noexcept
	{
		a = static_cast<FileModes>(a | b);
	}

	bool operator>(FileModes a, int b) noexcept
	{
		return static_cast<int>(a) > b;
	}
}