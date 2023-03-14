#include "ImageTypes.h"

namespace Coco::Rendering
{
	ImageUsageFlags operator|(ImageUsageFlags a, ImageUsageFlags b)
	{
		return static_cast<ImageUsageFlags>(static_cast<int>(a) | static_cast<int>(b));
	}

	ImageUsageFlags operator&(ImageUsageFlags a, ImageUsageFlags b)
	{
		return static_cast<ImageUsageFlags>(static_cast<int>(a) & static_cast<int>(b));
	}

	void operator|=(ImageUsageFlags& a, ImageUsageFlags b)
	{
		a = a | b;
	}

	void operator&=(ImageUsageFlags& a, ImageUsageFlags b)
	{
		a = a & b;
	}

	bool IsDepthStencilFormat(PixelFormat format)
	{
		return format == PixelFormat::Depth24_Stencil8;
	}

	int GetPixelFormatSize(PixelFormat format)
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