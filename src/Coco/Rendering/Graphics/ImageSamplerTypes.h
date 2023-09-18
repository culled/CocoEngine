#pragma once

#include <Coco/Core/Defines.h>

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

	/// @brief Mipmap filtering modes
	enum class MipMapFilterMode
	{
		Nearest,
		Linear
	};

	struct ImageSamplerDescription
	{
		ImageFilterMode MinimizeFilter;
		ImageFilterMode MagnifyFilter;

		ImageRepeatMode RepeatModeU;
		ImageRepeatMode RepeatModeV;
		ImageRepeatMode RepeatModeW;

		MipMapFilterMode MipMapFilter;

		uint8 MaxAnisotropy;

		ImageSamplerDescription();
		ImageSamplerDescription(ImageFilterMode filterMode, ImageRepeatMode repeatMode, MipMapFilterMode mipMapFilterMode, uint8 maxAnisotropy);
	};
}