#include "Image.h"

namespace Coco::Rendering
{
	Image::Image(ResourceID id, const string& name, ImageDescription description) noexcept : RenderingResource(id, name),
		Description(description)
	{}
}