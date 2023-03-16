#include "AttachmentDescription.h"

namespace Coco::Rendering
{
	const AttachmentDescription AttachmentDescription::Empty = {};

	AttachmentDescription::AttachmentDescription(Rendering::PixelFormat pixelFormat, Rendering::ColorSpace colorSpace, bool usesBlending) noexcept :
		PixelFormat(pixelFormat), ColorSpace(colorSpace), UsesBlending(usesBlending)
	{}

	bool AttachmentDescription::operator==(const AttachmentDescription& other) const noexcept
	{
		return PixelFormat == other.PixelFormat &&
			ColorSpace == other.ColorSpace &&
			UsesBlending == other.UsesBlending;
	}
}