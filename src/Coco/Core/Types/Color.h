#pragma once

#include "String.h"

namespace Coco
{
	/// @brief Represents a color composed of normalized R, G, B, and A decimal values
	struct Color
	{
		/// @brief The default gamma value to convert linear <-> sRGB
		static constexpr double DefaultGamma = 2.2;

		/// @brief A black color (0.0, 0.0, 0.0, 1.0)
		static const Color Black;

		/// @brief A mid-grey color (0.5, 0.5, 0.5, 1.0)
		static const Color MidGrey;

		/// @brief A white color (1.0, 1.0, 1.0, 1.0)
		static const Color White;

		/// @brief A red color (1.0, 0.0, 0.0, 1.0)
		static const Color Red;

		/// @brief A blue color (0.0, 0.0, 1.0, 1.0)
		static const Color Blue;

		/// @brief A green color (0.0, 1.0, 0.0, 1.0)
		static const Color Green;

		/// @brief A yellow color (1.0, 1.0, 0.0, 1.0)
		static const Color Yellow;

		/// @brief A magenta color (1.0, 0.0, 1.0, 1.0)
		static const Color Magenta;

		/// @brief A cyan color (0.0, 1.0, 1.0, 1.0)
		static const Color Cyan;

		/// @brief A fully-transparent black color (0.0, 0.0, 0.0, 0.0)
		static const Color ClearBlack;

		/// @brief A fully-transparent white color (1.0, 1.0, 1.0, 0.0)
		static const Color ClearWhite;

		/// @brief The normalized red component
		double R;

		/// @brief The normalized green component
		double G;

		/// @brief The normalized blue component
		double B;

		/// @brief The normalized alpha component
		double A;

		/// @brief True if this color represents a gamma value and should be converted to linear space before rendering
		bool IsLinear;

		Color();
		Color(double r, double g, double b, double a = 1.0, bool isLinear = false);

		/// @brief Performs a gamma to linear conversion for this color
		/// @param gamma The gamma value
		/// @return This color in linear color space
		Color AsLinear(double gamma = DefaultGamma) const;

		/// @brief Performs a linear to gamma conversion for this color
		/// @param gamma The gamma value
		/// @return This color in gamma color space
		Color AsGamma(double gamma = DefaultGamma) const;

		/// @brief Converts this color to linear color space
		/// @param gamma The gamma value
		void ConvertToLinear(double gamma = DefaultGamma);

		/// @brief Converts this color to gamma color space
		/// @param gamma The gamma value
		void ConvertToGamma(double gamma = DefaultGamma);

		/// @brief Converts this color to a string
		/// @return This color as a string
		string ToString() const;
	};
}