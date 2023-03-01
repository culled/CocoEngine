#pragma once

#include <Coco/Core/Core.h>

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A description for an attachment used by a render pass
	/// </summary>
	struct COCOAPI AttachmentDescription
	{
		/// <summary>
		/// An empty/unused attachment
		/// </summary>
		static const AttachmentDescription Empty;

		/// <summary>
		/// The pixel format of this attachment
		/// </summary>
		PixelFormat PixelFormat = PixelFormat::Unknown;

		/// <summary>
		/// The color space of this attachment
		/// </summary>
		ColorSpace ColorSpace = ColorSpace::Unknown;

		/// <summary>
		/// If true, this attachment uses blending of some sort
		/// </summary>
		bool UsesBlending = false;

		AttachmentDescription() = default;
		AttachmentDescription(Rendering::PixelFormat pixelFormat, Rendering::ColorSpace colorSpace, bool usesBlending);

		bool operator ==(const AttachmentDescription& other) const;
		bool operator !=(const AttachmentDescription& other) const;
	};
}