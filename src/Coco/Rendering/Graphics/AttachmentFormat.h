#pragma once

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// @brief Clear modes for an attachment
	enum class AttachmentClearMode
	{
		/// @brief The attachment should not be cleared
		Never,

		/// @brief The attachment should be cleared
		Clear
	};

	/// @brief A format for a rendering attachment
	struct AttachmentFormat
	{
		/// @brief The pixel format for this attachment
		ImagePixelFormat PixelFormat;

		/// @brief The color space for this attachment
		ImageColorSpace ColorSpace;
		
		/// @brief The clear mode for this attachment
		AttachmentClearMode ClearMode;

		/// @brief If true, this attachment's data will be preserved for subsequent renders
		bool PreserveAfterRender;

		AttachmentFormat();
		AttachmentFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, AttachmentClearMode clearMode, bool preserveAfterRender);

		/// @brief Determines if this AttachmentFormat is compatible with another
		/// @param other The other format
		/// @return True if the two formats are compatible
		bool IsCompatible(const AttachmentFormat& other) const;
	};
}