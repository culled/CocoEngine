#include "ImageSampler.h"

namespace Coco::Rendering
{
	ImageSampler::ImageSampler(
		ResourceID id, 
		const string& name, 
		const ImageSamplerProperties& properties
	) noexcept : 
		RenderingResource(id, name),
		_properties(properties)
	{}
}
