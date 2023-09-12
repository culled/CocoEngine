#include "Renderpch.h"
#include "ImageTypes.h"

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
}