#pragma once

#include "../../../Renderpch.h"
#include "VulkanIncludes.h"
#include "../../ShaderUniformLayout.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	/// @brief A layout for a Vulkan descriptor set
	struct VulkanDescriptorSetLayout
	{
		/// @brief The hash of the layout this descriptor set layout was created for
		uint64 LayoutHash;

		/// @brief The Vulkan descriptor set layout
		VkDescriptorSetLayout Layout;

		/// @brief The bindings for the Vulkan descriptor set
		std::vector<VkDescriptorSetLayoutBinding> LayoutBindings;

		/// @brief Templates for writing descriptor sets with this layout
		std::vector<VkWriteDescriptorSet> WriteTemplates;

		VulkanDescriptorSetLayout(uint64 layoutHash);

		/// @brief Creates a VulkanDescriptorSetLayout for the given ShaderUniformLayout
		/// @param device The device to create the descriptor set layout with
		/// @param layout The uniform layout
		/// @param includeDataUniforms If true, data uniforms will be included in the layout (if any exist)
		/// @return The created descriptor set layout
		static VulkanDescriptorSetLayout CreateForUniformLayout(VulkanGraphicsDevice& device, const ShaderUniformLayout& layout, bool includeDataUniforms);

		/// @brief Gets the count for a descriptor type in this layout
		/// @param descriptorType The type of descriptor to count
		/// @return The count of the descriptor type
		uint32 GetTypeCount(VkDescriptorType descriptorType) const;
	};
}