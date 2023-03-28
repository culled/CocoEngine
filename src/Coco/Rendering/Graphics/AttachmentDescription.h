#pragma once

#include <Coco/Core/API.h>

#include "Resources/ImageTypes.h"
#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	/// @brief A description for an attachment used by a render pass
	struct COCOAPI AttachmentDescription
	{
		/// @brief An empty/unused attachment
		static const AttachmentDescription Empty;

		/// @brief The pixel format of this attachment
		PixelFormat PixelFormat = PixelFormat::Unknown;

		/// @brief The color space of this attachment
		ColorSpace ColorSpace = ColorSpace::Unknown;

		/// @brief If true, this attachment should be preserved between frames
		bool ShouldPreserve = false;

		/// @brief The blend factor for the color source
		BlendFactorMode ColorSourceFactor = BlendFactorMode::One;

		/// @brief The blend factor for the color destination
		BlendFactorMode ColorDestinationFactor = BlendFactorMode::Zero;

		/// @brief The operation to use for combining the source and destination colors
		BlendOperation ColorBlendOperation = BlendOperation::None;

		/// @brief The blend factor for the alpha source
		BlendFactorMode AlphaSourceBlendFactor = BlendFactorMode::One;

		/// @brief The blend factor for the alpha destination
		BlendFactorMode AlphaDestinationBlendFactor = BlendFactorMode::Zero;

		/// @brief The operation to use for combining the source and destination alphas
		BlendOperation AlphaBlendOperation = BlendOperation::None;

		AttachmentDescription() = default;
		AttachmentDescription(Rendering::PixelFormat pixelFormat, Rendering::ColorSpace colorSpace, bool shouldPreserve) noexcept;

		bool operator ==(const AttachmentDescription& other) const noexcept;
		bool operator !=(const AttachmentDescription& other) const noexcept { return !(*this == other); }
	};
}