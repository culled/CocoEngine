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
		case ImagePixelFormat::R32_Int:
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
		case ImagePixelFormat::R32_Int:
			return 1;
		default:
			return 0;
		}
	}

	bool IsDepthFormat(ImagePixelFormat format)
	{
		switch (format)
		{
		case ImagePixelFormat::Depth32_Stencil8:
			return true;
		default:
			return false;
		}
	}

	bool IsStencilFormat(ImagePixelFormat format)
	{
		switch (format)
		{
		case ImagePixelFormat::Depth32_Stencil8:
			return true;
		default:
			return false;
		}
	}

	uint32 CalculateMipMapCount(uint32 width, uint32 height)
	{
		return static_cast<uint32>(Math::Floor(Math::Log2(Math::Max(width, height)))) + 1;
	}

	const ImageDescription ImageDescription::Empty = ImageDescription();

	ImageDescription::ImageDescription() : 
		ImageDescription(
			1, 1, 1, 
			0,
			ImagePixelFormat::Unknown,
			ImageColorSpace::Unknown,
			ImageUsageFlags::None,
			false)
	{}

	ImageDescription::ImageDescription(
		uint32 width, uint32 height,
		ImagePixelFormat pixelFormat,
		ImageColorSpace colorSpace,
		ImageUsageFlags usageFlags,
		bool withMipMaps,
		MSAASamples sampleCount) :
		ImageDescription(
			width, height, 1,
			1,
			pixelFormat,
			colorSpace,
			usageFlags,
			withMipMaps,
			sampleCount)
	{}

	ImageDescription::ImageDescription(
		uint32 width, uint32 height, uint32 depth,
		uint32 layers,
		ImagePixelFormat pixelFormat,
		ImageColorSpace colorSpace,
		ImageUsageFlags usageFlags,
		bool withMipMaps,
		MSAASamples sampleCount) :
		Width(Math::Max(static_cast<uint32>(1), width)),
		Height(Math::Max(static_cast<uint32>(1), height)),
		Depth(Math::Max(static_cast<uint32>(1), depth)),
		Layers(Math::Max(static_cast<uint32>(1), layers)),
		MipCount(1),
		PixelFormat(pixelFormat),
		ColorSpace(colorSpace),
		UsageFlags(usageFlags),
		SampleCount(sampleCount)
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

		if (withMipMaps)
		{
			MipCount = CalculateMipMapCount(Width, Height);
		}
	}
}