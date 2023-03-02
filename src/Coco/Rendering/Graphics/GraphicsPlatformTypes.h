#pragma once

#include <Coco/Core/Types/String.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Types of render hardward interfaces
	/// </summary>
	enum class RenderingRHI
	{
		Vulkan,
		DirectX12,
		OpenGL
	};

	/// <summary>
	/// Types of graphics devices
	/// </summary>
	enum class GraphicsDeviceType
	{
		Discrete,
		Integrated,
		CPU,
		Other
	};

	/// <summary>
	/// Vertical sync modes
	/// </summary>
	enum class VerticalSyncMode
	{
		Disabled,
		Enabled,
		Mailbox
	};
}