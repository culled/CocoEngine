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

	FileModes operator&(FileModes a, FileModes b);
	void operator&=(FileModes& a, FileModes b);
	FileModes operator|(FileModes a, FileModes b);
	void operator|=(FileModes& a, FileModes b);

	bool operator>(FileModes a, int b);
}