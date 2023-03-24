#pragma once

#include <Coco/Core/API.h>

namespace Coco
{
	struct Vector3;
	struct Vector4;

	/// @brief Represents a color composed of R, G, B, and A decimal values
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

		/// @brief The normalized red component
		double R = 0.0;

		/// @brief The normalized green component
		double G = 0.0;

		/// @brief The normalized blue component
		double B = 0.0;

		/// @brief The normalized alpha component
		double A = 1.0;

		Color() noexcept = default;
		Color(double r, double g, double b, double a = 1.0) noexcept;
		virtual ~Color() = default;

		operator Vector3() const noexcept;
		operator Vector4() const noexcept;
	};
}