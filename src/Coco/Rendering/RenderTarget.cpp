#include "RenderTarget.h"

namespace Coco::Rendering
{
	RenderTarget::RenderTarget() :
		Image(Ref<Rendering::Image>()), ClearColor(Color::Black)
	{
	}

	RenderTarget::RenderTarget(Ref<Rendering::Image> image, const Color& clearColor) :
		Image(image), ClearColor(clearColor)
	{
	}
}
