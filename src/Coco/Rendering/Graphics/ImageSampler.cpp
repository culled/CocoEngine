#include "ImageSampler.h"

namespace Coco::Rendering
{
	ImageSampler::ImageSampler(Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy) :
		FilterMode(filterMode), RepeatMode(repeatMode), MaxAnisotropy(maxAnisotropy)
	{}
}
