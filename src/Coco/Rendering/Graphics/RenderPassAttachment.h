#pragma once

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// @brief Usage flags for a RenderPassAttachment
	enum class AttachmentUsageFlags
	{
		None = 0,

		/// @brief The attachment will be used as a color attachment
		Color = 1 << 0,

		/// @brief The attachment will be used as a depth attachment
		Depth = 1 << 1,

		/// @brief The attachment will be used as a stencil attachment
		Stencil = 1 << 2,

		/// @brief The attachment will be used as a depth and stencil attachment
		DepthStencil = Depth | Stencil,

		/// @brief The attachment will be used as an input to the render pass
		Input = 1 << 3,
	};

	DefineFlagOperators(AttachmentUsageFlags)

	/// @brief Options flags for a RenderPassAttachment
	enum class AttachmentOptionsFlags
	{
		None = 0,

		/// @brief The attachment's contents will be stored between frames
		Preserve = 1 << 0,

		/// @brief The attachment will be cleared before it is rendered to
		Clear = 1 << 1,
	};

	DefineFlagOperators(AttachmentOptionsFlags)

	/// @brief An image attachment for a RenderPass
	struct RenderPassAttachment
	{
		/// @brief The pixel format for this attachment
		ImagePixelFormat PixelFormat;

		/// @brief The color space for this attachment
		ImageColorSpace ColorSpace;

		/// @brief Flags that define how this attachment is used
		AttachmentUsageFlags UsageFlags;

		/// @brief Flags that define options for this attachment
		AttachmentOptionsFlags OptionsFlags;

		RenderPassAttachment();
		RenderPassAttachment(
			ImagePixelFormat pixelFormat, 
			ImageColorSpace colorSpace, 
			AttachmentUsageFlags usageFlags, 
			AttachmentOptionsFlags optionsFlags = AttachmentOptionsFlags::None);

		bool operator==(const RenderPassAttachment& other) const;

		/// @brief Determines if this attachment's pixel format and color space are the same as another attachment's
		/// @param other The other attachment
		/// @return True if this attachment's pixel format and color space are the same as the given attachment
		bool IsCompatible(const RenderPassAttachment& other) const;

		bool IsCompatible(const ImageDescription& imageDescription) const;
	};
}