#include "Color.h"

#include "Vector.h"

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

	Color::Color(double r, double g, double b, double a, bool isLinear) noexcept :
		R(r), G(g), B(b), A(a), IsLinear(isLinear)
	{}

	Color Color::Parse(const string& str)
	{
		uint64_t currentCharacterIndex = 0;
		uint64_t fieldIndex = 0;
		Color c;

		while (currentCharacterIndex < str.length())
		{
			if (fieldIndex >= 5)
				break;

			uint64_t endIndex = str.find_first_of(',', currentCharacterIndex);

			if (endIndex == string::npos)
				endIndex = str.length();

			string part = str.substr(currentCharacterIndex, endIndex - currentCharacterIndex);

			if (fieldIndex < 4)
			{
				double v = atof(part.c_str());

				switch (fieldIndex)
				{
				case 0:
					c.R = v;
					break;
				case 1:
					c.G = v;
					break;
				case 2:
					c.B = v;
					break;
				case 3:
					c.A = v;
					break;
				default: break;
				}
			}
			else
			{
				c.IsLinear = atoi(part.c_str()) == 1;
			}

			currentCharacterIndex = endIndex + 1;
			fieldIndex++;
		}

		return c;
	}

	Color Color::AsLinear(double gamma) const
	{
		if (IsLinear)
			return *this;

		return Color(Math::Pow(R, gamma), Math::Pow(G, gamma), Math::Pow(B, gamma), true);
	}

	Color Color::AsGamma(double gamma) const
	{
		if (!IsLinear)
			return *this;

		return Color(Math::Pow(R, 1.0 / gamma), Math::Pow(G, 1.0 / gamma), Math::Pow(B, 1.0 / gamma), false);
	}

	Color::operator Vector3() const noexcept
	{
		return Vector3(R, G, B);
	}

	Color::operator Vector4() const noexcept
	{
		return Vector4(R, G, B, A);
	}
}