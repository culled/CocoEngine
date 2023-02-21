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
}