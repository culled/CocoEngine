#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Image pixel formats
	/// </summary>
	enum class PixelFormat
	{
		BGRA8,
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
		case PixelFormat::BGRA8:
			return 4;
		default:
			return 0;
		}
	}
}