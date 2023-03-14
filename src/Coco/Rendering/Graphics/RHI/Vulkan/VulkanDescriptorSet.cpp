#include "VulkanDescriptorSet.h"

namespace Coco::Rendering
{
    uint VulkanShaderDescriptorLayout::GetTypeCount(VkDescriptorType descriptorType) const
    {
        uint count = 0;

        for (uint i = 0; i < LayoutBindings.Count(); i++)
        {
            if (LayoutBindings[i].descriptorType == descriptorType)
                count++;
        }

        return count;
    }
}