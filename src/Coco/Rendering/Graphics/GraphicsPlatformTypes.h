#pragma once

#include "GraphicsDeviceTypes.h"

namespace Coco::Rendering
{
	/// @brief Types of render hardward interfaces
	enum class RenderingRHI
	{
		Vulkan,
		DirectX12,
		OpenGL
	};

	/// @brief Converts an RenderingRHI to a string
	/// @param rhi The render hardware interface
	/// @return The string equivalent of the RHI
	constexpr const char* PlatformRHIToString(RenderingRHI rhi) noexcept
	{
		switch (rhi)
		{
		case RenderingRHI::Vulkan:
			return "Vulkan";
		case RenderingRHI::DirectX12:
			return "DirectX 12";
		case RenderingRHI::OpenGL:
			return "Open GL";
		default:
			return "Unknown";
		}
	}

	/// @brief Parameters for creating a graphics platform
	struct COCOAPI GraphicsPlatformCreationParameters
	{
		/// @brief The name of the application
		string ApplicationName;

		/// @brief The type of render hardware interface to use
		RenderingRHI RHI;

		/// @brief Parameters for creating the graphics device
		GraphicsDeviceCreationParameters DeviceCreateParams = {};

		GraphicsPlatformCreationParameters(const string& applicationName, RenderingRHI rhi) : ApplicationName(applicationName), RHI(rhi) {}
	};
}