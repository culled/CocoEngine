#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Types of render hardward interfaces
	/// </summary>
	enum class COCOAPI RenderingRHI
	{
		Vulkan,
		DirectX12,
		OpenGL
	};

	/// <summary>
	/// Types of graphics devices
	/// </summary>
	enum class COCOAPI GraphicsDeviceType
	{
		Discrete,
		Integrated,
		CPU,
		Other
	};
}