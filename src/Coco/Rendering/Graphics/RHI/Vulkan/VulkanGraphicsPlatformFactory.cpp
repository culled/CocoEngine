#include "Renderpch.h"
#include "VulkanGraphicsPlatformFactory.h"
#include "VulkanGraphicsPlatform.h"
#include "VulkanUtils.h"

namespace Coco::Rendering::Vulkan
{
	const Version VulkanGraphicsPlatformFactory::sDefaultAPIVersion = Version(1, 2, 0);

	VulkanGraphicsPlatformFactory::VulkanGraphicsPlatformFactory(const GraphicsPlatformCreateParams& createParams, const Version& apiVersion, bool useValidationLayers) :
		GraphicsPlatformFactory(createParams),
		_apiVersion(ToVkVersion(apiVersion)),
		_useValidationLayers(useValidationLayers)
	{}

	UniqueRef<GraphicsPlatform> VulkanGraphicsPlatformFactory::Create() const
	{
		return CreateUniqueRef<VulkanGraphicsPlatform>(_createParams, _apiVersion, _useValidationLayers);
	}
}