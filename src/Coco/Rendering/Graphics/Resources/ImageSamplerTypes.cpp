#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	const ImageSamplerProperties ImageSamplerProperties::Default = ImageSamplerProperties(ImageFilterMode::Linear, ImageRepeatMode::Repeat, 16);

	ImageSamplerProperties::ImageSamplerProperties() : FilterMode(ImageFilterMode::Linear), RepeatMode(ImageRepeatMode::Repeat), MaxAnisotropy(16)
	{}

	ImageSamplerProperties::ImageSamplerProperties(Rendering::ImageFilterMode filterMode, ImageRepeatMode repeatMode, uint maxAnisotropy) :
		FilterMode(filterMode), RepeatMode(repeatMode), MaxAnisotropy(maxAnisotropy)
	{}
}
