#include "Image.h"

#include "GraphicsDevice.h"

namespace Coco::Rendering
{
	const ImageDescription ImageDescription::Empty = ImageDescription();

	ImageDescription::ImageDescription(int width, int height, int depth, 
		int layers, int mipCount, 
		Rendering::PixelFormat pixelFormat, 
		Rendering::ColorSpace colorSpace,
		ImageUsageFlags usageFlags) noexcept :
		Height(Math::Max(height, 1)),
		Width(Math::Max(width, 1)),
		Depth(Math::Max(depth, 1)),
		Layers(Math::Max(layers, 1)),
		MipCount(Math::Max(mipCount, 1)),
		PixelFormat(pixelFormat),
		ColorSpace(colorSpace),
		UsageFlags(usageFlags)
	{
		if (depth == 1)
		{
			if (height == 1)
			{
				DimensionType = layers == 1 ? ImageDimensionType::OneDimensional : ImageDimensionType::OneDimensionalArray;
			}
			else
			{
				DimensionType = layers == 1 ? ImageDimensionType::TwoDimensional : ImageDimensionType::TwoDimensionalArray;
			}
		}
		else
		{
			DimensionType = layers == 1 ? ImageDimensionType::ThreeDimensional : ImageDimensionType::ThreeDimensionalArray;
		}
	}

	Image::Image(ImageDescription description) noexcept :
		Description(description)
	{}
}