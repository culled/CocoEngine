#include "AttachmentDescription.h"

namespace Coco::Rendering
{
	const AttachmentDescription AttachmentDescription::Empty = {};

	AttachmentDescription::AttachmentDescription(Rendering::PixelFormat pixelFormat, Rendering::ColorSpace colorSpace, bool usesBlending) noexcept :
		PixelFormat(pixelFormat), ColorSpace(colorSpace), UsesBlending(usesBlending)
	{
	}

	bool AttachmentDescription::operator==(const AttachmentDescription& other) const noexcept
	{
		return this->PixelFormat == other.PixelFormat &&
			this->ColorSpace == other.ColorSpace &&
			this->UsesBlending == other.UsesBlending;
	}

	bool AttachmentDescription::operator!=(const AttachmentDescription& other) const noexcept
	{
		return !(*this == other);
	}
}