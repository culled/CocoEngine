#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// <summary>
	/// Layout for a Vulkan descriptor set
	/// </summary>
	struct VulkanDescriptorLayout
	{
		VkDescriptorSetLayout Layout = nullptr;
		List<VkDescriptorSetLayoutBinding> LayoutBindings;

		/// <summary>
		/// Gets the count for a descriptor type in this layout
		/// </summary>
		/// <param name="descriptorType">The type of descriptor to count</param>
		/// <returns>The count of the descriptor type</returns>
		uint GetTypeCount(VkDescriptorType descriptorType) const noexcept;
	};
}