#include "Size.h"

#include "Vector.h"

namespace Coco
{
	const SizeInt SizeInt::Zero = SizeInt(0, 0);

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

	const Size Size::Zero = Size(0.0, 0.0);

	Size::Size(double width, double height) noexcept : Width(width), Height(height)
	{}

	Size::operator Vector2() const noexcept
	{
		return Vector2(Width, Height);
	}
}