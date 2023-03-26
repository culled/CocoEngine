#pragma once

#include <Coco/Core/API.h>

namespace Coco::Rendering
{
	/// @brief Image pixel formats
	enum class PixelFormat
	{
		/// @brief R, G, B, and A channels, each storing 8 bits
		RGBA8,

		/// @brief 24 bit depth channel with an 8 bit stencil channel
		Depth24_Stencil8,

		/// @brief An unknown format
		Unknown
	};

	/// @brief Image color spaces
	enum class ColorSpace
	{
		sRGB,
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
		None				= 0,
		TransferSource		= 1 << 0,
		TransferDestination = 1 << 1,
		Sampled				= 1 << 2,
		RenderTarget		= 1 << 3,
		Presented			= 1 << 4,
	};

	COCOAPI ImageUsageFlags operator|(ImageUsageFlags a, ImageUsageFlags b) noexcept;
	COCOAPI ImageUsageFlags operator&(ImageUsageFlags a, ImageUsageFlags b) noexcept;
	COCOAPI void operator|=(ImageUsageFlags& a, ImageUsageFlags b) noexcept;
	COCOAPI void operator&=(ImageUsageFlags& a, ImageUsageFlags b) noexcept;

	/// @brief Gets if the given pixel format is a depth/stencil format
	/// @param format The format
	/// @return True if the format is a depth/stencil format
	constexpr bool IsDepthStencilFormat(PixelFormat format) noexcept
	{
		return format == PixelFormat::Depth24_Stencil8;
	}

	/// @brief Gets the number of bytes per pixel for a pixel format
	/// @param format The pixel format
	/// @return The number of bytes per pixel
	constexpr uint GetPixelFormatSize(PixelFormat format) noexcept
	{
		switch (format)
		{
		case PixelFormat::RGBA8:
		case PixelFormat::Depth24_Stencil8:
			return 4;
		default:
			return 0;
		}
	}

	/// @brief Gets the number of channels for a pixel format
	/// @param format The pixel format
	/// @return The number of channels
	constexpr uint GetPixelFormatChannelCount(PixelFormat format) noexcept
	{
		switch (format)
		{
		case PixelFormat::RGBA8:
			return 4;
		case PixelFormat::Depth24_Stencil8:
			return 2;
		default:
			return 0;
		}
	}

	/// @brief A description of an Image
	struct COCOAPI ImageDescription
	{
		/// @brief An empty image description
		static const ImageDescription Empty;

		/// @brief The pixel width of the image
		int Width = 1;

		/// @brief The pixel height of the image
		int Height = 1;

		/// @brief The pixel depth of the image
		int Depth = 1;

		/// @brief The number of layers in this image
		int Layers = 1;

		/// @brief The number of mip maps this image has
		int MipCount = 1;

		/// @brief The pixel format of this image
		PixelFormat PixelFormat = PixelFormat::Unknown;

		/// @brief The color space of this image
		ColorSpace ColorSpace = ColorSpace::Unknown;

		/// @brief The type of dimensions of this image
		ImageDimensionType DimensionType = ImageDimensionType::TwoDimensional;

		/// @brief Usage flags for this image
		ImageUsageFlags UsageFlags = ImageUsageFlags::None;

		ImageDescription() = default;
		ImageDescription(int width, int height,
			int mipCount,
			Rendering::PixelFormat pixelFormat,
			Rendering::ColorSpace colorSpace,
			ImageUsageFlags usageFlags) noexcept :
			ImageDescription(width, height, 1, 1, mipCount, pixelFormat, colorSpace, usageFlags) {}
		ImageDescription(
			int width, int height, int depth,
			int layers, int mipCount,
			Rendering::PixelFormat pixelFormat,
			Rendering::ColorSpace colorSpace,
			ImageUsageFlags usageFlags) noexcept;
	};
}