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
		Color c(*this);
		c.ConvertToLinear();
		return c;
	}

	Color Color::AsGamma(double gamma) const
	{
		Color c(*this);
		c.ConvertToGamma();
		return c;
	}

	void Color::ConvertToLinear(double gamma)
	{
		if (IsLinear)
			return;

		R = Math::Pow(R, gamma);
		G = Math::Pow(G, gamma);
		B = Math::Pow(B, gamma);
		IsLinear = true;
	}

	void Color::ConvertToGamma(double gamma)
	{
		if (!IsLinear)
			return;

		R = Math::Pow(R, 1.0 / gamma);
		G = Math::Pow(G, 1.0 / gamma);
		B = Math::Pow(B, 1.0 / gamma);
		IsLinear = false;
	}

	string Color::ToString() const
	{
		return FormatString("{}, {}, {}, {}, {}", R, G, B, A, IsLinear);
	}
}