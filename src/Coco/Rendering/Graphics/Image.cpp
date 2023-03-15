#include "Image.h"

#include "GraphicsDevice.h"

namespace Coco::Rendering
{
	const ImageDescription ImageDescription::Empty = ImageDescription();

	ImageDescription::ImageDescription(int width, int height, 
		int mipCount, 
		Rendering::PixelFormat pixelFormat, 
		Rendering::ColorSpace colorSpace,
		ImageUsageFlags usageFlags) noexcept :
		ImageDescription(width, height, 1, 1, mipCount, pixelFormat, colorSpace, usageFlags)
	{}

	ImageDescription::ImageDescription(int width, int height, int depth, 
		int layers, int mipCount, 
		Rendering::PixelFormat pixelFormat, 
		Rendering::ColorSpace colorSpace,
		ImageUsageFlags usageFlags) noexcept :
		Height(std::max(height, 1)),
		Width(std::max(width, 1)),
		Depth(std::max(depth, 1)),
		Layers(std::max(layers, 1)),
		MipCount(std::max(mipCount, 1)),
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