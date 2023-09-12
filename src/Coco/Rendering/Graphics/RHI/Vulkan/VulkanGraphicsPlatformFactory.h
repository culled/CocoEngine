#pragma once

#include "../../GraphicsPlatformFactory.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief A GraphicsPlatformFactory that creates a VulkanGraphicsPlatform
	class VulkanGraphicsPlatformFactory : public GraphicsPlatformFactory
	{
	public:
		VulkanGraphicsPlatformFactory(const GraphicsPlatformCreateParams& createParams);

		UniqueRef<GraphicsPlatform> Create() const final;
	};
}

