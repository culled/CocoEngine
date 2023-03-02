#include "GraphicsPlatform.h"

#include <Coco/Rendering/RenderingService.h>

#if COCO_RENDERING_VULKAN
#include "RHI/Vulkan/GraphicsPlatformVulkan.h"
#endif

namespace Coco::Rendering
{
	GraphicsPlatform::GraphicsPlatform(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams) :
		RenderService(renderingService), SupportsPresentation(creationParams.DeviceCreateParams.SupportsPresentation)
	{}

	string GraphicsPlatform::PlatformRHIToString(RenderingRHI rhi)
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

	Managed<GraphicsPlatform> GraphicsPlatform::CreatePlatform(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams)
	{
		switch (creationParams.RHI)
		{
#if COCO_RENDERING_VULKAN
		case RenderingRHI::Vulkan:
			return CreateManaged<GraphicsPlatformVulkan>(renderingService, creationParams);
#endif
		default:
			break;
		}

		string err = FormattedString("{} RHI is not supported", PlatformRHIToString(creationParams.RHI));
		throw Exception(err.c_str());
	}
}