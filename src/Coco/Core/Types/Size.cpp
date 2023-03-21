#include "Size.h"

#include "Vector.h"

namespace Coco
{
	SizeInt::SizeInt(int width, int height) noexcept : Width(width), Height(height)
	{}

	SizeInt::operator Vector2Int() const noexcept
	{
		return Vector2Int(Width, Height);
	}

	SizeInt::operator Vector2() const noexcept
	{
		return Vector2(Width, Height);
	}

	Size::Size(double width, double height) noexcept : Width(width), Height(height)
	{}

	Size::operator Vector2() const noexcept
	{
		return Vector2(Width, Height);
	}
}