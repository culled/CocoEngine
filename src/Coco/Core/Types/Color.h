#pragma once

#include <Coco/Core/API.h>

namespace Coco
{
	struct Vector3;
	struct Vector4;

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
		double R = 0.0;

		/// <summary>
		/// The normalized green component
		/// </summary>
		double G = 0.0;

		/// <summary>
		/// The normalized blue component
		/// </summary>
		double B = 0.0;

		/// <summary>
		/// The normalized alpha component
		/// </summary>
		double A = 1.0;

		Color() noexcept = default;
		Color(double r, double g, double b, double a = 1.0) noexcept;
		virtual ~Color() = default;

		operator Vector3() const noexcept;
		operator Vector4() const noexcept;
	};
}