//
// Created by cullen on 3/8/26.
//

#ifndef COCOENGINE_COLOR_H
#define COCOENGINE_COLOR_H
#include "Coco/Core/Math/Vector4.h"

namespace Coco
{
    /// @brief Represents a Color that has a red, green, blue, and alpha values
	struct Color
	{
		/// @brief The default gamma value to convert linear <-> sRGB
		static constexpr float DefaultGamma = 2.2f;

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

		float Raw[4];

		/// @brief True if this color represents a gamma value and should be converted to linear space before rendering
		bool IsGamma;

		Color()  noexcept :
			Color(0.0f, 0.0f, 0.0f, 1.0f, true)
		{}

		Color(float r, float g, float b, float a = 1.0, bool isGamma = true) noexcept;
		Color(const Vector4& color, bool isGamma);

		constexpr float& R() { return Raw[0]; }
		constexpr const float& R() const { return Raw[0]; }

		constexpr float& G() { return Raw[1]; }
		constexpr const float& G() const { return Raw[1]; }

		constexpr float& B() { return Raw[2]; }
		constexpr const float& B() const { return Raw[2]; }

		constexpr float& A() { return Raw[3]; }
		constexpr const float& A() const { return Raw[3]; }

		/// @brief Performs a gamma to linear conversion for this color and returns the converted color
		/// @param gamma The gamma value
		/// @return This color in linear color space
		Color AsLinear(float gamma = DefaultGamma) const noexcept;

		/// @brief Performs a linear to gamma conversion for this color and returns the converted color
		/// @param gamma The gamma value
		/// @return This color in gamma color space
		Color AsGamma(float gamma = DefaultGamma) const noexcept;

		/// @brief Gets a copy of this color with the given alpha value
		/// @param alpha The alpha value for the color
		/// @return This color with the given alpha
		Color WithAlpha(float alpha) const noexcept;

		/// @brief Converts this color to linear color space
		/// @param gamma The gamma value
		void ConvertToLinear(float gamma = DefaultGamma) noexcept;

		/// @brief Converts this color to gamma color space
		/// @param gamma The gamma value
		void ConvertToGamma(float gamma = DefaultGamma) noexcept;

		Vector4 AsVector4(bool gammaCorrected) const noexcept;
	};
} // Coco

#endif //COCOENGINE_COLOR_H