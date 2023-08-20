#include "AttachmentDescription.h"

namespace Coco::Rendering
{
	const AttachmentDescription AttachmentDescription::Empty = {};

	AttachmentDescription::AttachmentDescription(
		Rendering::PixelFormat pixelFormat, 
		Rendering::ColorSpace colorSpace, 
		bool shouldPreserve
	) noexcept :
		PixelFormat(pixelFormat), ColorSpace(colorSpace), ShouldPreserve(shouldPreserve)
	{}

	bool AttachmentDescription::operator==(const AttachmentDescription& other) const noexcept
	{
		return PixelFormat == other.PixelFormat &&
			ColorSpace == other.ColorSpace &&
			ShouldPreserve == other.ShouldPreserve &&
			ColorSourceFactor == other.ColorSourceFactor &&
			ColorDestinationFactor == other.ColorDestinationFactor &&
			ColorBlendOperation == other.ColorBlendOperation &&
			AlphaSourceBlendFactor == other.AlphaSourceBlendFactor &&
			AlphaDestinationBlendFactor == other.AlphaDestinationBlendFactor && 
			AlphaBlendOperation == other.AlphaBlendOperation;
	}

	bool AttachmentDescription::IsCompatible(const AttachmentDescription& other) const noexcept
	{
		return PixelFormat == other.PixelFormat;
	}
}