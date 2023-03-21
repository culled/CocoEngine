#pragma once

#include "GraphicsDeviceTypes.h"

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
	/// Parameters for creating a graphics platform
	/// </summary>
	struct COCOAPI GraphicsPlatformCreationParameters
	{
		/// <summary>
		/// The name of the application
		/// </summary>
		string ApplicationName;

		/// <summary>
		/// The type of render hardware interface to use
		/// </summary>
		RenderingRHI RHI;

		/// <summary>
		/// Parameters for creating the graphics device
		/// </summary>
		GraphicsDeviceCreationParameters DeviceCreateParams = {};

		GraphicsPlatformCreationParameters(const string& applicationName, RenderingRHI rhi) : ApplicationName(applicationName), RHI(rhi) {}
	};
}