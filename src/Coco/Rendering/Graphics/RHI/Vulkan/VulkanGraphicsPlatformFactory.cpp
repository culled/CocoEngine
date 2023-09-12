#include "Renderpch.h"
#include "VulkanGraphicsPlatformFactory.h"
#include "VulkanGraphicsPlatform.h"

namespace Coco::Rendering::Vulkan
{
	VulkanGraphicsPlatformFactory::VulkanGraphicsPlatformFactory(const GraphicsPlatformCreateParams& createParams) :
		GraphicsPlatformFactory(createParams)
	{}

	UniqueRef<GraphicsPlatform> VulkanGraphicsPlatformFactory::Create() const
	{
		return CreateUniqueRef<VulkanGraphicsPlatform>(_createParams);
	}
}