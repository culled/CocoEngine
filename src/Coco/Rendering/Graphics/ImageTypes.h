#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	enum class PixelFormat
	{
		B8G8R8A8_UNorm,
		D24UNorm_S8UInt,
		Unknown
	};

	enum class ColorSpace
	{
		sRGB,
		Unknown
	};

	enum class ImageType
	{
		OneDimensional,
		TwoDimensional,
		TwoDimensionalArray,
		ThreeDimensional,
		CubeMap,
		CubeMapArray
	};
}