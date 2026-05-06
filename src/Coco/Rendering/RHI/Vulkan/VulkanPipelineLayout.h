//
// Created by cullen on 3/30/26.
//

#ifndef COCOENGINE_VULKANPIPELINELAYOUT_H
#define COCOENGINE_VULKANPIPELINELAYOUT_H
#include "VulkanDescriptorSetLayout.h"

namespace Coco
{
    struct VulkanPipelineLayout
    {
        Array<VulkanDescriptorSetLayout> DescriptorSetLayouts;
        Array<VkPushConstantRange> PushConstantRanges;
        uint32 GlobalDescriptorSetIndexOffset;
        VkPipelineLayout PipelineLayout;

        VulkanPipelineLayout();
    };
} // Coco

#endif //COCOENGINE_VULKANPIPELINELAYOUT_H