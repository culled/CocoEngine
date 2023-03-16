#include "GraphicsPlatform.h"

#include <Coco/Rendering/RenderingService.h>

#if COCO_RENDERING_VULKAN
#include "RHI/Vulkan/GraphicsPlatformVulkan.h"
#endif

namespace Coco::Rendering
{
	GraphicsPlatform::GraphicsPlatform(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams) noexcept :
		RenderService(renderingService), SupportsPresentation(creationParams.DeviceCreateParams.SupportsPresentation)
	{}

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

		throw GraphicsPlatformCreateException(FormattedString("{} RHI is not supported", PlatformRHIToString(creationParams.RHI)));
	}
}