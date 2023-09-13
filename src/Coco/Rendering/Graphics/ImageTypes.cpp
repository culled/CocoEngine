#include "Renderpch.h"
#include "ImageTypes.h"
#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	bool AreCompatible(ImagePixelFormat a, ImagePixelFormat b)
	{
		return GetPixelFormatSize(a) == GetPixelFormatSize(b) &&
			GetPixelFormatChannelCount(a) == GetPixelFormatChannelCount(b);
	}

	uint8 GetPixelFormatSize(ImagePixelFormat format)
	{
		switch (format)
		{
		case ImagePixelFormat::RGBA8:
			return 4;
		case ImagePixelFormat::Depth32_Stencil8:
			return 5;
		default:
			return 0;
		}
	}

	uint8 GetPixelFormatChannelCount(ImagePixelFormat format)
	{
		switch (format)
		{
		case ImagePixelFormat::RGBA8:
			return 4;
		case ImagePixelFormat::Depth32_Stencil8:
			return 2;
		default:
			return 0;
		}
	}

	const ImageDescription ImageDescription::Empty = ImageDescription();

	ImageDescription::ImageDescription() : 
		ImageDescription(
			0, 0, 0, 
			0, 0,
			ImagePixelFormat::Unknown,
			ImageColorSpace::Unknown,
			ImageUsageFlags::None)
	{}

	ImageDescription::ImageDescription(
		uint32 width, uint32 height,
		uint32 mipCount,
		ImagePixelFormat pixelFormat,
		ImageColorSpace colorSpace,
		ImageUsageFlags usageFlags) :
		ImageDescription(
			width, height, 1,
			1, mipCount,
			pixelFormat,
			colorSpace,
			usageFlags)
	{}

	ImageDescription::ImageDescription(
		uint32 width, uint32 height, uint32 depth,
		uint32 layers, uint32 mipCount,
		ImagePixelFormat pixelFormat,
		ImageColorSpace colorSpace,
		ImageUsageFlags usageFlags) :
		Width(Math::Max(static_cast<uint32>(1), width)),
		Height(Math::Max(static_cast<uint32>(1), height)),
		Depth(Math::Max(static_cast<uint32>(1), depth)),
		Layers(Math::Max(static_cast<uint32>(1), layers)),
		MipCount(Math::Max(static_cast<uint32>(1), mipCount)),
		PixelFormat(pixelFormat),
		ColorSpace(colorSpace),
		UsageFlags(usageFlags)
	{
		if (Depth <= 1)
		{
			if (Height <= 1)
			{
				DimensionType = Layers == 1 ? ImageDimensionType::OneDimensional : ImageDimensionType::OneDimensionalArray;
			}
			else
			{
				DimensionType = Layers == 1 ? ImageDimensionType::TwoDimensional : ImageDimensionType::TwoDimensionalArray;
			}
		}
		else
		{
			DimensionType = Layers == 1 ? ImageDimensionType::ThreeDimensional : ImageDimensionType::ThreeDimensionalArray;
		}
	}
}