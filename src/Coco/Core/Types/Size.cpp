#include "Corepch.h"
#include "Size.h"

#include "Vector.h"

namespace Coco
{
	const Size Size::Zero = Size(0, 0);

	Size::Size() : Size(0, 0)
	{}

	Size::Size(double width, double height) :
		Width(width),
		Height(height)
	{}

	Size::operator Vector2() const { return Vector2(Width, Height); }

	string Size::ToString() const
	{
		return FormatString("{}x{}", Width, Height);
	}

	const SizeInt SizeInt::Zero = SizeInt(0, 0);

	SizeInt::SizeInt() : SizeInt(0, 0)
	{}

	SizeInt::SizeInt(int width, int height) :
		Width(width),
		Height(height)
	{}

	SizeInt::operator Vector2Int() const { return Vector2Int(Width, Height); }

	string SizeInt::ToString() const
	{
		return FormatString("{}x{}", Width, Height);
	}
}