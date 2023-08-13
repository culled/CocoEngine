#pragma once

#include <Coco/Core/API.h>
#include "String.h"

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

		/// @brief True if this color represents a gamma value and should be converted to linear space before rendering
		bool IsLinear = false;

		Color() noexcept = default;
		Color(double r, double g, double b, double a = 1.0, bool isLinear = false) noexcept;
		virtual ~Color() = default;

		/// @brief Parses a Color from a string
		/// @param str The string
		/// @return The parsed Color
		static Color Parse(const string& str);

		/// @brief Performs a gamma to linear conversion for this color
		/// @param gamma The gamma value
		/// @return This color in linear color space
		Color AsLinear(double gamma = 2.2) const;

		/// @brief Performs a linear to gamma conversion for this color
		/// @param gamma The gamma value
		/// @return This color in gamma color space
		Color AsGamma(double gamma = 2.2) const;

		/// @brief Converts this color to a string
		/// @return This color as a string
		string ToString() const { return FormattedString("{}, {}, {}, {}, {:b}", R, G, B, A, IsLinear); }

		operator Vector3() const noexcept;
		operator Vector4() const noexcept;
	};
}