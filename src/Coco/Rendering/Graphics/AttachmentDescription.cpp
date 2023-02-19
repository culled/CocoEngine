#include "AttachmentDescription.h"

namespace Coco::Rendering
{
	const AttachmentDescription AttachmentDescription::Empty = {};

	bool AttachmentDescription::operator==(const AttachmentDescription& other) const
	{
		return this->PixelFormat == other.PixelFormat &&
			this->ColorSpace == other.ColorSpace &&
			this->UsesBlending == other.UsesBlending;
	}

	bool AttachmentDescription::operator!=(const AttachmentDescription& other) const
	{
		return !(*this == other);
	}
}