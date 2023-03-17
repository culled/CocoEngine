#include "VulkanDescriptorSet.h"

namespace Coco::Rendering::Vulkan
{
    uint VulkanDescriptorLayout::GetTypeCount(VkDescriptorType descriptorType) const noexcept
    {
        uint count = 0;

        for(const VkDescriptorSetLayoutBinding& binding : LayoutBindings)
            if (binding.descriptorType == descriptorType)
                count++;

        return count;
    }
}