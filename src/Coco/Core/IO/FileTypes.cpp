#include "FileTypes.h"

namespace Coco
{
	FileModes operator&(FileModes a, FileModes b)
	{
		return static_cast<FileModes>(static_cast<int>(a) & static_cast<int>(b));
	}

	void operator&=(FileModes& a, FileModes b)
	{
		a = static_cast<FileModes>(a & b);
	}

	FileModes operator|(FileModes a, FileModes b)
	{
		return static_cast<FileModes>(static_cast<int>(a) | static_cast<int>(b));
	}

	void operator|=(FileModes& a, FileModes b)
	{
		a = static_cast<FileModes>(a | b);
	}

	bool operator>(FileModes a, int b)
	{
		return static_cast<int>(a) > b;
	}
}