#include "Corepch.h"
#include "Color.h"

#include "../Math/Math.h"

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
	const Color Color::ClearBlack = Color(0.0, 0.0, 0.0, 0.0);
	const Color Color::ClearWhite = Color(1.0, 1.0, 1.0, 0.0);

	Color::Color() : Color(0, 0, 0, 0, false)
	{}

	Color::Color(double r, double g, double b, double a, bool isLinear) :
		R(r),
		G(g),
		B(b),
		A(a),
		IsLinear(isLinear)
	{}

	Color Color::AsLinear(double gamma) const
	{
		if (IsLinear)
			return *this;

		return Color(Math::Pow(R, gamma), Math::Pow(G, gamma), Math::Pow(B, gamma), A, true);
	}

	Color Color::AsGamma(double gamma) const
	{
		if (!IsLinear)
			return *this;

		return Color(Math::Pow(R, 1.0 / gamma), Math::Pow(G, 1.0 / gamma), Math::Pow(B, 1.0 / gamma), A, false);
	}

	string Color::ToString() const
	{
		return FormatString("{}, {}, {}, {}, {}", R, G, B, A, IsLinear);
	}
}