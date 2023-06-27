#include "Image.h"

namespace Coco::Rendering
{
	Image::Image(ResourceID id, const string& name, uint64_t lifetime, ImageDescription description) noexcept : RenderingResource(id, name, lifetime),
		Description(description)
	{}
}