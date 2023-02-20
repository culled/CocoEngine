#include "GraphicsPlatform.h"

#include <Coco/Rendering/RenderingService.h>

#if COCO_RENDERING_VULKAN
#include "RHI/Vulkan/GraphicsPlatformVulkan.h"
#endif

namespace Coco::Rendering
{
	GraphicsPlatform::GraphicsPlatform(RenderingService* renderingService, const GraphicsBackendCreationParameters& creationParams) :
		RenderService(renderingService), SupportsPresentation(creationParams.DeviceCreateParams.SupportsPresentation)
	{}

	GraphicsPlatform* GraphicsPlatform::CreatePlatform(RenderingService* renderingService, const GraphicsBackendCreationParameters& creationParams)
	{
		switch (creationParams.RHI)
		{
#if COCO_RENDERING_VULKAN
		case RenderingRHI::Vulkan:
			return new GraphicsPlatformVulkan(renderingService, creationParams);
#endif
		default:
			throw Exception("Unsupported rendering interface");
		}
	}
}