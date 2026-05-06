//
// Created by cullen on 3/30/26.
//

#include "VulkanPipelineLayout.h"
#include "VulkanIncludes.h"

namespace Coco
{
    VulkanPipelineLayout::VulkanPipelineLayout() :
        GlobalDescriptorSetIndexOffset(0),
        PipelineLayout(nullptr)
    {}
} // Coco