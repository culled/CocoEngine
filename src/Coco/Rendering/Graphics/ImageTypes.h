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

	/// @brief Image dimension types
	enum class ImageDimensionType
	{
		OneDimensional,
		OneDimensionalArray,
		TwoDimensional,
		TwoDimensionalArray,
		ThreeDimensional,
		ThreeDimensionalArray,
		CubeMap,
		CubeMapArray
	};

	/// @brief Usage flags for an image
	enum class ImageUsageFlags
	{
		None = 0,
		TransferSource = 1 << 0,
		TransferDestination = 1 << 1,
		Sampled = 1 << 2,
		RenderTarget = 1 << 3,
		Presented = 1 << 4,
	};

	constexpr ImageUsageFlags operator|(ImageUsageFlags a, ImageUsageFlags b) { return static_cast<ImageUsageFlags>(static_cast<int>(a) | static_cast<int>(b)); }
	constexpr ImageUsageFlags operator&(ImageUsageFlags a, ImageUsageFlags b) { return static_cast<ImageUsageFlags>(static_cast<int>(a) & static_cast<int>(b)); }

	constexpr void operator|=(ImageUsageFlags& a, ImageUsageFlags b) { a = a | b; }
	constexpr void operator&=(ImageUsageFlags& a, ImageUsageFlags b) { a = a & b; }

	/// @brief A description of an Image
	struct ImageDescription
	{
		/// @brief An empty image description
		static const ImageDescription Empty;

		/// @brief The pixel width of the image
		uint32 Width;

		/// @brief The pixel height of the image
		uint32 Height;

		/// @brief The pixel depth of the image
		uint32 Depth;

		/// @brief The number of layers in this image
		uint32 Layers;

		/// @brief The number of mip maps this image has
		uint32 MipCount;

		/// @brief The pixel format of this image
		ImagePixelFormat PixelFormat;

		/// @brief The color space of this image
		ImageColorSpace ColorSpace;

		/// @brief The type of dimensions of this image
		ImageDimensionType DimensionType;

		/// @brief Usage flags for this image
		ImageUsageFlags UsageFlags;

		ImageDescription();

		ImageDescription(
			uint32 width, uint32 height,
			uint32 mipCount,
			ImagePixelFormat pixelFormat,
			ImageColorSpace colorSpace,
			ImageUsageFlags usageFlags);

		ImageDescription(
			uint32 width, uint32 height, uint32 depth,
			uint32 layers, uint32 mipCount,
			ImagePixelFormat pixelFormat,
			ImageColorSpace colorSpace,
			ImageUsageFlags usageFlags);

		bool operator==(const ImageDescription& other) const;
	};
}