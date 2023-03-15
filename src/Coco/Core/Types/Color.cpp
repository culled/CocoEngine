#include "Color.h"

namespace Coco
{
	const Color Color::Black = Color(0.0, 0.0, 0.0, 1.0);
	const Color Color::White = Color(1.0, 1.0, 1.0, 1.0);
	const Color Color::Red = Color(1.0, 0.0, 0.0, 1.0);
	const Color Color::Green = Color(0.0, 1.0, 0.0, 1.0);
	const Color Color::Blue = Color(0.0, 0.0, 1.0, 1.0);
	const Color Color::Yellow = Color(1.0, 1.0, 0.0, 1.0);
	const Color Color::Magenta = Color(1.0, 0.0, 1.0, 1.0);
	const Color Color::Cyan = Color(0.0, 1.0, 1.0, 1.0);
	const Color Color::Clear = Color(0.0, 0.0, 0.0, 0.0);

	Color::Color() noexcept : Color(0.0, 0.0, 0.0, 0.0)
	{
	}

	Color::Color(double r, double g, double b, double a) noexcept :
		R(r), G(g), B(b), A(a)
	{
	}
}