#pragma once

#include <Coco/Core/Core.h>
#include "Graphics/Resources/Image.h"
#include <Coco/Core/Types/Color.h>

namespace Coco::Rendering
{
	struct RenderTarget
	{
		Ref<Rendering::Image> Image;
		Color ClearColor;

		RenderTarget();
		RenderTarget(Ref<Rendering::Image> image, const Color& clearColor);
	};
}