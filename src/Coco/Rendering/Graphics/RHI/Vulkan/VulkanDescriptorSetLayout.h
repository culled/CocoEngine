#pragma once

#include "../../../Renderpch.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief A layout for a Vulkan descriptor set
	struct VulkanDescriptorSetLayout
	{
		/// @brief The Vulkan descriptor set layout
		VkDescriptorSetLayout Layout;

		/// @brief The bindings for the Vulkan descriptor set
		std::vector<VkDescriptorSetLayoutBinding> LayoutBindings;

		VulkanDescriptorSetLayout();

		/// @brief Gets the count for a descriptor type in this layout
		/// @param descriptorType The type of descriptor to count
		/// @return The count of the descriptor type
		uint32 GetTypeCount(VkDescriptorType descriptorType) const;
	};
}