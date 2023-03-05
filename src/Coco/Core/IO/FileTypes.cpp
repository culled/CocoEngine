#include "FileTypes.h"

namespace Coco
{
	int operator&(FileModes a, FileModes b)
	{
		return static_cast<int>(a) & static_cast<int>(b);
	}

	void operator&=(FileModes& a, FileModes b)
	{
		a = static_cast<FileModes>(a & b);
	}

	int operator|(FileModes a, FileModes b)
	{
		return static_cast<int>(a) | static_cast<int>(b);
	}

	void operator|=(FileModes& a, FileModes b)
	{
		a = static_cast<FileModes>(a | b);
	}
}