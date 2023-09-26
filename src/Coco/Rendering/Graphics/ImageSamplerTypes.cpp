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
		uint8 maxAnisotropy) :
		MinimizeFilter(filterMode),
		MagnifyFilter(filterMode),
		RepeatModeU(repeatMode),
		RepeatModeV(repeatMode),
		RepeatModeW(repeatMode),
		MipMapFilter(mipMapFilterMode),
		MaxAnisotropy(maxAnisotropy),
		MinLOD(0),
		MaxLOD(0),
		LODBias(0.0)
	{}
}