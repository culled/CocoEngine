#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	struct COCOAPI Color
	{
		static const Color Black;
		static const Color White;
		static const Color Red;
		static const Color Blue;
		static const Color Green;
		static const Color Yellow;
		static const Color Magenta;
		static const Color Cyan;
		static const Color Clear;

		double R;
		double G;
		double B;
		double A;

		Color();
		Color(double r, double g, double b, double a = 1.0);
	};
}