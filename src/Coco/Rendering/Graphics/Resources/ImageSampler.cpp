#include "ImageSampler.h"

namespace Coco::Rendering
{
	ImageSampler::ImageSampler(
		ResourceID id, 
		const string& name, 
		uint64_t lifetime, 
		const ImageSamplerProperties& properties
	) noexcept : 
		RenderingResource(id, name, lifetime),
		_properties(properties)
	{}
}
