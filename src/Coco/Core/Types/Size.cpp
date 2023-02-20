#include "Size.h"

namespace Coco
{
	SizeInt::SizeInt() : SizeInt(0, 0)
	{}

	SizeInt::SizeInt(int width, int height) : Width(width), Height(height)
	{}

	SizeInt SizeInt::operator+(const SizeInt & other) const
	{
		return SizeInt(this->Width + other.Width, this->Height + other.Height);
	}

	SizeInt SizeInt::operator-(const SizeInt& other) const
	{
		return SizeInt(this->Width - other.Width, this->Height - other.Height);
	}

	bool SizeInt::operator==(const SizeInt& other) const
	{
		return this->Width == other.Width && this->Height == other.Height;
	}

	bool SizeInt::operator!=(const SizeInt& other) const
	{
		return !(*this == other);
	}
}