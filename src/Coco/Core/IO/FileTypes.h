#pragma once

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

	int operator&(FileModes a, FileModes b);
	void operator&=(FileModes& a, FileModes b);
	int operator|(FileModes a, FileModes b);
	void operator|=(FileModes& a, FileModes b);
}