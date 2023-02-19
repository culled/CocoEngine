#pragma once

#include <Coco/Core/Core.h>

#include "ImageTypes.h"

namespace Coco::Rendering
{
	struct COCOAPI AttachmentDescription
	{
		static const AttachmentDescription Empty;

		PixelFormat PixelFormat = PixelFormat::Unknown;
		ColorSpace ColorSpace = ColorSpace::Unknown;

		bool UsesBlending = false;

		bool operator ==(const AttachmentDescription& other) const;
		bool operator !=(const AttachmentDescription& other) const;
	};
}