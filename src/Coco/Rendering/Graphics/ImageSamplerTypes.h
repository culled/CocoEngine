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

	/// @brief An ImageSampler description
	struct ImageSamplerDescription
	{
		/// @brief The filter to use when an image is minimized on-screen
		ImageFilterMode MinimizeFilter;

		/// @brief The filter to use when an image is magnified on-screen
		ImageFilterMode MagnifyFilter;

		/// @brief The repeat mode in the U direction
		ImageRepeatMode RepeatModeU;

		/// @brief The repeat mode in the V direction
		ImageRepeatMode RepeatModeV;

		/// @brief The repeat mode in the W direction
		ImageRepeatMode RepeatModeW;

		/// @brief The filter to use between mip map levels
		MipMapFilterMode MipMapFilter;

		/// @brief The maximum anisotropy to use for filtering
		uint8 MaxAnisotropy;

		/// @brief The minimum mip map level to use
		uint32 MinLOD;

		/// @brief The maximum mip map level to use
		uint32 MaxLOD;

		/// @brief The bias for the mip map lod
		double LODBias;

		ImageSamplerDescription();
		ImageSamplerDescription(ImageFilterMode filterMode, ImageRepeatMode repeatMode, MipMapFilterMode mipMapFilterMode, uint8 maxAnisotropy);
	};
}