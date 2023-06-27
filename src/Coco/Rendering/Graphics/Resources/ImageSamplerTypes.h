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

	struct ImageSamplerProperties
	{
		static const ImageSamplerProperties Default;

		ImageFilterMode FilterMode;
		ImageRepeatMode RepeatMode;
		uint MaxAnisotropy;

		ImageSamplerProperties();
		ImageSamplerProperties(ImageFilterMode filterMode, ImageRepeatMode repeatMode, uint maxAnisotropy);
	};
}