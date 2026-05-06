//
// Created by cullen on 3/29/26.
//
#include "ImageSamplerTypes.h"

namespace Coco
{
    const ImageSamplerDescription ImageSamplerDescription::LinearRepeat(ImageFilterMode::Linear, ImageRepeatMode::Repeat, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::LinearClamp(ImageFilterMode::Linear, ImageRepeatMode::Clamp, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::LinearMirror(ImageFilterMode::Linear, ImageRepeatMode::Mirror, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::NearestRepeat(ImageFilterMode::Nearest, ImageRepeatMode::Repeat, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::NearestClamp(ImageFilterMode::Nearest, ImageRepeatMode::Clamp, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::NearestMirror(ImageFilterMode::Nearest, ImageRepeatMode::Mirror, MipMapFilterMode::Linear, 0);

	ImageSamplerDescription::ImageSamplerDescription() noexcept :
		ImageSamplerDescription(
			ImageFilterMode::Linear,
			ImageRepeatMode::Repeat,
			MipMapFilterMode::Linear,
			0)
	{}

	ImageSamplerDescription::ImageSamplerDescription(
		ImageFilterMode filterMode,
		ImageRepeatMode repeatMode,
		MipMapFilterMode mipMapFilterMode,
		uint32 maxAnisotropy) noexcept :
		ImageSamplerDescription(filterMode, filterMode,
			repeatMode, repeatMode, repeatMode,
			mipMapFilterMode,
			maxAnisotropy,
			0, 0, 0.0)
	{}

	ImageSamplerDescription::ImageSamplerDescription(
		ImageFilterMode minimizeFilter, ImageFilterMode magnifyFilter,
		ImageRepeatMode repeatModeU, ImageRepeatMode repeatModeV, ImageRepeatMode repeatModeW,
		MipMapFilterMode mipMapFilterMode,
		uint32 maxAnisotropy,
		uint32 minLOD, uint32 maxLOD, float lodBias) noexcept :
		MinimizeFilter(minimizeFilter),
		MagnifyFilter(magnifyFilter),
		RepeatModeU(repeatModeU),
		RepeatModeV(repeatModeV),
		RepeatModeW(repeatModeW),
		MipMapFilter(mipMapFilterMode),
		MaxAnisotropy(maxAnisotropy),
		MinLOD(minLOD),
		MaxLOD(maxLOD),
		LODBias(lodBias)
	{}
}