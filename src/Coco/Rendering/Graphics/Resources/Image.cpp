#include "Image.h"

namespace Coco::Rendering
{
	Image::Image(const ResourceID& id, const string& name, ImageDescription description) noexcept : RenderingResource(id, name),
		_description(description)
	{}
}