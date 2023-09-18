#include "Renderpch.h"
#include "VulkanDescriptorSetLayout.h"

namespace Coco::Rendering::Vulkan
{
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout() :
		Layout(nullptr),
		LayoutBindings{}
	{}

	uint32 VulkanDescriptorSetLayout::GetTypeCount(VkDescriptorType descriptorType) const
	{
		uint32 count = 0;

		for (const VkDescriptorSetLayoutBinding& binding : LayoutBindings)
			if (binding.descriptorType == descriptorType)
				count++;

		return count;
	}
}