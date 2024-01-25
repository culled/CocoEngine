#include "Renderpch.h"
#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	const ImageSamplerDescription ImageSamplerDescription::LinearRepeat = ImageSamplerDescription(ImageFilterMode::Linear, ImageRepeatMode::Repeat, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::NearestRepeat = ImageSamplerDescription(ImageFilterMode::Nearest, ImageRepeatMode::Repeat, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::LinearClamp = ImageSamplerDescription(ImageFilterMode::Linear, ImageRepeatMode::Clamp, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::NearestClamp = ImageSamplerDescription(ImageFilterMode::Nearest, ImageRepeatMode::Clamp, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::LinearMirror = ImageSamplerDescription(ImageFilterMode::Linear, ImageRepeatMode::Mirror, MipMapFilterMode::Linear, 0);
	const ImageSamplerDescription ImageSamplerDescription::NearestMirror = ImageSamplerDescription(ImageFilterMode::Nearest, ImageRepeatMode::Mirror, MipMapFilterMode::Linear, 0);

	ImageSamplerDescription::ImageSamplerDescription(
		ImageFilterMode filterMode,
		ImageRepeatMode repeatMode,
		MipMapFilterMode mipMapFilterMode,
		uint32 maxAnisotropy) :
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
		uint32 minLOD, uint32 maxLOD, double lodBias) :
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