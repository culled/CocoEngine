#pragma once

#include <Coco/Core/Core.h>
#include "Graphics/Resources/Image.h"
#include <Coco/Core/Types/Color.h>

namespace Coco::Rendering
{
	/// @brief An image that can be rendered to
	struct RenderTarget
	{
		/// @brief The image
		Ref<Rendering::Image> Image;

		/// @brief The clear color for when rendering begins
		Color ClearColor;

		RenderTarget();
		RenderTarget(Ref<Rendering::Image> image, const Color& clearColor);
	};
}