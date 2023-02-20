#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Image pixel formats
	/// </summary>
	enum class PixelFormat
	{
		B8G8R8A8_UNorm,
		D24UNorm_S8UInt,
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
		TwoDimensional,
		TwoDimensionalArray,
		ThreeDimensional,
		CubeMap,
		CubeMapArray
	};
}