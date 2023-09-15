#pragma once

#include "ImageTypes.h"
#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	/// @brief A format for a rendering attachment
	struct AttachmentFormat
	{
		/// @brief The pixel format for this attachment
		ImagePixelFormat PixelFormat;
		
		/// @brief If true, this attachment will be preserved between frames
		bool ShouldPreserve;

		/// @brief The number of MSAA samples
		MSAASamples SampleCount;

		AttachmentFormat();
		AttachmentFormat(ImagePixelFormat pixelFormat, bool shouldPreserve, MSAASamples msaaSamples = MSAASamples::One);

		/// @brief Determines if this AttachmentFormat is compatible with another
		/// @param other The other format
		/// @return True if the two formats are compatible
		bool IsCompatible(const AttachmentFormat& other) const;
	};
}