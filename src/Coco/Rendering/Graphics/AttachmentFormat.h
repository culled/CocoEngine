#pragma once

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// @brief A format for a rendering attachment
	struct AttachmentFormat
	{
		/// @brief The pixel format for this attachment
		ImagePixelFormat PixelFormat;

		/// @brief The color space for this attachment
		ImageColorSpace ColorSpace;
		
		/// @brief If true, this attachment will be cleared before rendering
		bool ShouldClear;

		AttachmentFormat();
		AttachmentFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, bool shouldClear);

		/// @brief Determines if this AttachmentFormat is compatible with another
		/// @param other The other format
		/// @return True if the two formats are compatible
		bool IsCompatible(const AttachmentFormat& other) const;

		/// @brief Determines if this AttachmentFormat is compatible with an image description
		/// @param description The image description
		/// @return True if the image description is compatible with this attachment format
		bool IsCompatible(const ImageDescription& description) const;
	};
}