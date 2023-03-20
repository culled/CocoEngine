#include "ImageTypes.h"

namespace Coco::Rendering
{
	ImageUsageFlags operator|(ImageUsageFlags a, ImageUsageFlags b) noexcept
	{
		return static_cast<ImageUsageFlags>(static_cast<int>(a) | static_cast<int>(b));
	}

	ImageUsageFlags operator&(ImageUsageFlags a, ImageUsageFlags b) noexcept
	{
		return static_cast<ImageUsageFlags>(static_cast<int>(a) & static_cast<int>(b));
	}

	void operator|=(ImageUsageFlags& a, ImageUsageFlags b) noexcept
	{
		a = a | b;
	}

	void operator&=(ImageUsageFlags& a, ImageUsageFlags b) noexcept
	{
		a = a & b;
	}
}