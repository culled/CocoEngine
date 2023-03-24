#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Types/List.h>
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief Layout for a Vulkan descriptor set
	struct VulkanDescriptorLayout
	{
		/// @brief The Vulkan descriptor set layout
		VkDescriptorSetLayout Layout = nullptr;

		/// @brief The bindings for the Vulkan descriptor set
		List<VkDescriptorSetLayoutBinding> LayoutBindings;

		/// @brief Gets the count for a descriptor type in this layout
		/// @param descriptorType The type of descriptor to count
		/// @return The count of the descriptor type
		uint GetTypeCount(VkDescriptorType descriptorType) const noexcept;
	};
}