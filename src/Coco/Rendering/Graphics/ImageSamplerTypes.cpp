#include "Renderpch.h"
#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	ImageSamplerDescription::ImageSamplerDescription() :
		ImageSamplerDescription(ImageFilterMode::Linear, ImageRepeatMode::Repeat, MipMapFilterMode::Linear, 0)
	{}

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