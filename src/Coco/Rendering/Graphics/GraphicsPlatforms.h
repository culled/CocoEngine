#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	enum class COCOAPI RenderingRHI
	{
		Vulkan,
		DirectX12,
		OpenGL
	};
}