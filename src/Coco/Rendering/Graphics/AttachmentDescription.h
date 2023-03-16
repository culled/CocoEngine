#pragma once

#include <Coco/Core/Core.h>

#include "ImageTypes.h"
#include "GraphicsPipelineTypes.h"

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

		/// <summary>
		/// The blend factor for the color source
		/// </summary>
		BlendFactorMode ColorSourceFactor = BlendFactorMode::One;

		/// <summary>
		/// The blend factor for the color destination
		/// </summary>
		BlendFactorMode ColorDestinationFactor = BlendFactorMode::Zero;

		/// <summary>
		/// The operation to use for combining the source and destination colors
		/// </summary>
		BlendOperation ColorBlendOperation = BlendOperation::Add;

		/// <summary>
		/// The blend factor for the alpha source
		/// </summary>
		BlendFactorMode AlphaSourceBlendFactor = BlendFactorMode::One;

		/// <summary>
		/// The blend factor for the alpha destination
		/// </summary>
		BlendFactorMode AlphaDestinationBlendFactor = BlendFactorMode::Zero;

		/// <summary>
		/// The operation to use for combining the source and destination alphas
		/// </summary>
		BlendOperation AlphaBlendOperation = BlendOperation::Add;

		AttachmentDescription() = default;
		AttachmentDescription(Rendering::PixelFormat pixelFormat, Rendering::ColorSpace colorSpace, bool usesBlending) noexcept;

		bool operator ==(const AttachmentDescription& other) const noexcept;
		bool operator !=(const AttachmentDescription& other) const noexcept { return !(*this == other); }
	};
}