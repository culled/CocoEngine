#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Holds a color composed of R, G, B, and A decimal values
	/// </summary>
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

		/// <summary>
		/// The normalized red component
		/// </summary>
		double R;

		/// <summary>
		/// The normalized green component
		/// </summary>
		double G;

		/// <summary>
		/// The normalized blue component
		/// </summary>
		double B;

		/// <summary>
		/// The normalized alpha component
		/// </summary>
		double A;

		Color();
		Color(double r, double g, double b, double a = 1.0);
	};
}