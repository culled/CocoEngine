#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Image pixel formats
	/// </summary>
	enum class PixelFormat
	{
		RGBA8,
		Depth24_Stencil8,
		Unknown
	};

	/// <summary>
	/// Image color spaces
	/// </summary>
	enum class ColorSpace
	{
		sRGB,
		Unknown
	};

	/// <summary>
	/// Image dimension types
	/// </summary>
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

	enum class ImageUsageFlags
	{
		None				= 0,
		TransferSource		= 1 << 0,
		TransferDestination = 1 << 1,
		Sampled				= 1 << 2,
		RenderTarget		= 1 << 3,
	};

	COCOAPI ImageUsageFlags operator|(ImageUsageFlags a, ImageUsageFlags b) noexcept;
	COCOAPI ImageUsageFlags operator&(ImageUsageFlags a, ImageUsageFlags b) noexcept;
	COCOAPI void operator|=(ImageUsageFlags& a, ImageUsageFlags b) noexcept;
	COCOAPI void operator&=(ImageUsageFlags& a, ImageUsageFlags b) noexcept;

	/// <summary>
	/// Gets if the given pixel format is a depth/stencil format
	/// </summary>
	/// <param name="format">The format</param>
	/// <returns>True if the format is a depth/stencil format</returns>
	constexpr bool IsDepthStencilFormat(PixelFormat format) noexcept
	{
		return format == PixelFormat::Depth24_Stencil8;
	}

	/// <summary>
	/// Gets the number of bytes per pixel for a pixel format
	/// </summary>
	/// <param name="format">The pixel format</param>
	/// <returns>The number of bytes per pixel</returns>
	constexpr uint GetPixelFormatSize(PixelFormat format) noexcept
	{
		switch (format)
		{
		case PixelFormat::RGBA8:
			return 4;
		default:
			return 0;
		}
	}

	/// <summary>
	/// Gets the number of channels for a pixel format
	/// </summary>
	/// <param name="format">The pixel format</param>
	/// <returns>The number of channels</returns>
	constexpr uint GetPixelFormatChannelCount(PixelFormat format) noexcept
	{
		switch (format)
		{
		case PixelFormat::RGBA8:
			return 4;
		default:
			return 0;
		}
	}

	/// <summary>
	/// A description of an Image
	/// </summary>
	struct COCOAPI ImageDescription
	{
		/// <summary>
		/// An empty image description
		/// </summary>
		static const ImageDescription Empty;

		/// <summary>
		/// The pixel width of the image
		/// </summary>
		int Width = 1;

		/// <summary>
		/// The pixel height of the image
		/// </summary>
		int Height = 1;

		/// <summary>
		/// The pixel depth of the image
		/// </summary>
		int Depth = 1;

		/// <summary>
		/// The number of layers in this image
		/// </summary>
		int Layers = 1;

		/// <summary>
		/// The number of mip maps this image has
		/// </summary>
		int MipCount = 1;

		/// <summary>
		/// The pixel format of this image
		/// </summary>
		PixelFormat PixelFormat = PixelFormat::Unknown;

		/// <summary>
		/// The color space of this image
		/// </summary>
		ColorSpace ColorSpace = ColorSpace::Unknown;

		/// <summary>
		/// The type of dimensions of this image
		/// </summary>
		ImageDimensionType DimensionType = ImageDimensionType::TwoDimensional;

		/// <summary>
		/// Usage flags for this image
		/// </summary>
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