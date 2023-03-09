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

	/// <summary>
	/// Gets if the given pixel format is a depth/stencil format
	/// </summary>
	/// <param name="format">The format</param>
	/// <returns>True if the format is a depth/stencil format</returns>
	COCOAPI bool IsDepthStencilFormat(PixelFormat format);
}