//
// Created by cullen on 3/30/26.
//

#ifndef COCOENGINE_VULKANPIPELINELAYOUTBUILDER_H
#define COCOENGINE_VULKANPIPELINELAYOUTBUILDER_H

#include "Coco/Core/Types/Array.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanPipelineLayout.h"
#include "VulkanIncludes.h"
#include <slang.h>

namespace Coco
{
    class VulkanGraphicsPlatform;

    class VulkanPipelineLayoutBuilder
    {
        friend class VulkanDescriptorSetLayoutBuilder;
    public:
        VulkanPipelineLayoutBuilder(VulkanGraphicsPlatform* platform);

        VulkanPipelineLayout BuildForProgram(slang::ProgramLayout* programLayout);

    private:
        VulkanGraphicsPlatform* _platform;
        VkShaderStageFlags _currentShaderStage;
        VulkanPipelineLayout _pipelineLayout;

        void AddDescriptorSetForParameterBlock(slang::TypeLayoutReflection* parameterBlockTypeLayout);
        void AddSubObjectRanges(slang::TypeLayoutReflection* typeLayout);
        void AddSubObjectRange(slang::TypeLayoutReflection* typeLayout, long subObjectRangeIndex);
        void AddPushConstantRange(slang::TypeLayoutReflection* typeLayout);
    };
} // Coco

#endif //COCOENGINE_VULKANPIPELINELAYOUTBUILDER_H