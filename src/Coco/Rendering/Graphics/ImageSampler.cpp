#include "ImageSampler.h"

namespace Coco::Rendering
{
	ImageSampler::ImageSampler(Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy) noexcept :
		FilterMode(filterMode), RepeatMode(repeatMode), MaxAnisotropy(maxAnisotropy)
	{}
}
