#pragma once

#include <Coco/Core/Defines.h>

namespace Coco::Rendering
{
	/// @brief Image pixel formats
	enum class ImagePixelFormat
	{
		/// @brief R, G, B, and A channels, each storing 8 bits
		RGBA8,

		/// @brief 32 bit depth channel with an 8 bit stencil channel
		Depth32_Stencil8,

		/// @brief An unknown format
		Unknown
	};

	/// @brief Determines if two ImagePixelFormats are compatible with each other
	/// @param a The first pixel format
	/// @param b The second pixel format
	/// @return True if the two formats are compatible
	bool AreCompatible(ImagePixelFormat a, ImagePixelFormat b);

	/// @brief Gets the number of bytes per pixel for a pixel format
	/// @param format The pixel format
	/// @return The number of bytes per pixel
	uint8 GetPixelFormatSize(ImagePixelFormat format);

	/// @brief Gets the number of channels for a pixel format
	/// @param format The pixel format
	/// @return The number of channels
	uint8 GetPixelFormatChannelCount(ImagePixelFormat format);

	/// @brief Image color spaces
	enum class ImageColorSpace
	{
		sRGB,
		Linear,
		Unknown
	};
}