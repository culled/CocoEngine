#pragma once

#include <Coco/Core/API.h>

namespace Coco::Rendering
{
	/// @brief Image sampler filtering modes
	enum class ImageFilterMode
	{
		Nearest,
		Linear
	};

	/// @brief Image sampler repeat modes
	enum class ImageRepeatMode
	{
		Repeat,
		Clamp,
		Mirror,
		MirrorOnce
	};

	/// @brief Properties for an ImageSampler
	struct ImageSamplerProperties
	{
		/// @brief The default sampler properties
		static const ImageSamplerProperties Default;

		/// @brief The image filtering mode
		ImageFilterMode FilterMode;

		/// @brief The repeat mode for sampling beyond the image bounds
		ImageRepeatMode RepeatMode;

		/// @brief The maximum anisotropy to sample the image
		uint MaxAnisotropy;

		ImageSamplerProperties();
		ImageSamplerProperties(ImageFilterMode filterMode, ImageRepeatMode repeatMode, uint maxAnisotropy);
	};
}