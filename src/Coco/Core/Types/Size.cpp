#include "Size.h"

namespace Coco
{
	SizeInt::SizeInt(int width, int height) noexcept : Width(width), Height(height)
	{}

	SizeInt SizeInt::operator+(const SizeInt & other) const noexcept
	{
		return SizeInt(this->Width + other.Width, this->Height + other.Height);
	}

	SizeInt SizeInt::operator-(const SizeInt& other) const noexcept
	{
		return SizeInt(this->Width - other.Width, this->Height - other.Height);
	}

	bool SizeInt::operator==(const SizeInt& other) const noexcept
	{
		return this->Width == other.Width && this->Height == other.Height;
	}

	bool SizeInt::operator!=(const SizeInt& other) const noexcept
	{
		return !(*this == other);
	}
}