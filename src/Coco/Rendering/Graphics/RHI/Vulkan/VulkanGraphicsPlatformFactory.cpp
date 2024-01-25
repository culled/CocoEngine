#include "Renderpch.h"
#include "VulkanGraphicsPlatformFactory.h"
#include "VulkanGraphicsPlatform.h"

namespace Coco::Rendering::Vulkan
{
	VulkanGraphicsDeviceCreateParams::VulkanGraphicsDeviceCreateParams() :
		PreferredDeviceType(GraphicsDeviceType::Discrete),
		PresentationSupport(true),
		RequireComputeCapability(true),
		RequireTransferCapability(true),
		EnableAnisotropicSampling(true),
		EnableDepthClamping(false),
		EnableWireframeDrawing(false)
	{}

	const Version VulkanGraphicsPlatformCreateParams::DefaultAPIVersion = Version(1, 3, 0);

	VulkanGraphicsPlatformCreateParams::VulkanGraphicsPlatformCreateParams(const Application& app, bool presentationSupport) :
		App(app),
		PresentationSupport(presentationSupport),
		APIVersion(DefaultAPIVersion),
		UseValidationLayers(false),
		RenderingExtensions(),
		DeviceCreateParams()
	{}

	VulkanGraphicsPlatformFactory::VulkanGraphicsPlatformFactory(const VulkanGraphicsPlatformCreateParams& createParams) :
		_createParams(createParams)
	{}

	UniqueRef<GraphicsPlatform> VulkanGraphicsPlatformFactory::Create() const
	{
		return CreateUniqueRef<VulkanGraphicsPlatform>(_createParams);
	}
}