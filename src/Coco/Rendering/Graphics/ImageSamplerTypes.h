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
		/// @brief A sampler that uses linear filtering and repeats the image on each axis
		static const ImageSamplerDescription LinearRepeat;

		/// @brief A sampler that uses nearest filtering and repeats the image on each axis
		static const ImageSamplerDescription NearestRepeat;

		/// @brief A sampler that uses linear filtering and clamps the image at the border on each axis
		static const ImageSamplerDescription LinearClamp;

		/// @brief A sampler that uses nearest filtering and clamps the image at the border on each axis
		static const ImageSamplerDescription NearestClamp;

		/// @brief A sampler that uses linear filtering and mirrors the image on each axis
		static const ImageSamplerDescription LinearMirror;

		/// @brief A sampler that uses nearest filtering and mirrors the image on each axis
		static const ImageSamplerDescription NearestMirror;

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
		uint32 MaxAnisotropy;

		/// @brief The minimum mip map level to use
		uint32 MinLOD;

		/// @brief The maximum mip map level to use
		uint32 MaxLOD;

		/// @brief The bias for the mip map lod
		double LODBias;

		ImageSamplerDescription(ImageFilterMode filterMode, ImageRepeatMode repeatMode, MipMapFilterMode mipMapFilterMode, uint32 maxAnisotropy);
		ImageSamplerDescription(ImageFilterMode minimizeFilter, ImageFilterMode magnifyFilter,
			ImageRepeatMode repeatModeU, ImageRepeatMode repeatModeV, ImageRepeatMode repeatModeW,
			MipMapFilterMode mipMapFilterMode,
			uint32 maxAnisotropy,
			uint32 minLOD, uint32 maxLOD, double lodBias);
	};
}