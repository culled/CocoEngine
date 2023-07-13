#include "GraphicsPlatform.h"

#include <Coco/Rendering/RenderingService.h>
#include "GraphicsDevice.h"

#if COCO_RENDERING_VULKAN
#include "RHI/Vulkan/GraphicsPlatformVulkan.h"
#endif

namespace Coco::Rendering
{
	const Matrix4x4 GraphicsPlatform::CocoViewToRenderView({
			1.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, -1.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 1.0 });

	GraphicsPlatform::GraphicsPlatform( const GraphicsPlatformCreationParameters& creationParams) noexcept :
		SupportsPresentation(creationParams.DeviceCreateParams.SupportsPresentation)
	{}

	ManagedRef<GraphicsPlatform> GraphicsPlatform::CreatePlatform(const GraphicsPlatformCreationParameters& creationParams)
	{
		switch (creationParams.RHI)
		{
#if COCO_RENDERING_VULKAN
		case RenderingRHI::Vulkan:
			return CreateManagedRef<Vulkan::GraphicsPlatformVulkan>(creationParams);
#endif
		default:
			break;
		}

		throw GraphicsPlatformInitializeException(FormattedString("{} RHI is not supported", PlatformRHIToString(creationParams.RHI)));
	}

	void GraphicsPlatform::PurgeResource(const Ref<Resource>& resource, bool forcePurge)
	{
		GetDevice()->PurgeResource(resource, forcePurge);
	}
}