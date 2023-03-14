#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	struct VulkanShaderDescriptorLayout
	{
		VkDescriptorSetLayout Layout = nullptr;
		List<VkDescriptorSetLayoutBinding> LayoutBindings;

		uint GetTypeCount(VkDescriptorType descriptorType) const;
	};
}