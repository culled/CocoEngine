#pragma once

#include "../../GraphicsPlatformFactory.h"
#include <Coco/Core/Types/Version.h>

namespace Coco::Rendering::Vulkan
{
	/// @brief A GraphicsPlatformFactory that creates a VulkanGraphicsPlatform
	class VulkanGraphicsPlatformFactory : public GraphicsPlatformFactory
	{
	public:
		static const Version sDefaultAPIVersion;

	private:
		uint32 _apiVersion;

	public:
		VulkanGraphicsPlatformFactory(const GraphicsPlatformCreateParams& createParams, const Version& apiVersion = sDefaultAPIVersion);

		UniqueRef<GraphicsPlatform> Create() const final;
	};
}

