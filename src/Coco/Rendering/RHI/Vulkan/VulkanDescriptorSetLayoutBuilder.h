//
// Created by cullen on 3/30/26.
//

#ifndef COCOENGINE_VULKANDESCRIPTORSETLAYOUTBUILDER_H
#define COCOENGINE_VULKANDESCRIPTORSETLAYOUTBUILDER_H
#include "Coco/Core/Types/Array.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanIncludes.h"
#include <slang.h>

namespace Coco
{
    class VulkanGraphicsPlatform;
    class VulkanPipelineLayoutBuilder;

    class VulkanDescriptorSetLayoutBuilder
    {
    public:
        VulkanDescriptorSetLayoutBuilder(VulkanPipelineLayoutBuilder& pipelineLayoutBuilder);

        void AddDescriptorRanges(slang::TypeLayoutReflection* elementTypeLayout);

        VulkanDescriptorSetLayout FinishBuilding(VulkanGraphicsPlatform& platform);

    private:
        VulkanPipelineLayoutBuilder* _pipelineLayoutBuilder;
        VulkanDescriptorSetLayout _layout;

    private:
        void AddAutomaticallyInducedUniformBuffer();
        void AddRanges(slang::TypeLayoutReflection* typeLayout);
        void AddDescriptorRange(slang::TypeLayoutReflection* typeLayout, int rangeIndex);
    };
} // Coco

#endif //COCOENGINE_VULKANDESCRIPTORSETLAYOUTBUILDER_H