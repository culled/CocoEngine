//
// Created by cullen on 3/26/26.
//

#ifndef COCOENGINE_VULKANDESCRIPTORSETLAYOUT_H
#define COCOENGINE_VULKANDESCRIPTORSETLAYOUT_H

#include "Coco/Core/Types/Array.h"
#include "VulkanForwardDeclarations.h"
#include "Coco/Rendering/ShaderTypes.h"

namespace Coco
{
    class VulkanGraphicsPlatform;

    struct VulkanDescriptorSetLayout
    {
        Array<VkDescriptorSetLayoutBinding> LayoutBindings;
        ShaderStageFlags ShaderStages;
        uint32 DescriptorSetIndex;
        VkDescriptorSetLayout DescriptorSetLayout;
    /*public:
        VulkanDescriptorSetLayout(VulkanGraphicsPlatform* platform);
        ~VulkanDescriptorSetLayout();

        Span<const VkDescriptorSetLayoutBinding> GetLayoutBindings() const;
        VkDescriptorSetLayout GetDescriptorSetLayout() const { return _descriptorSetLayout; }
        bool IsUsed() const { return !_layoutBindings.IsEmpty(); }

    private:
        VulkanGraphicsPlatform* _platform;
        Array<VkDescriptorSetLayoutBinding> _layoutBindings;
        VkDescriptorSetLayout _descriptorSetLayout;*/
    };
} // Coco

#endif //COCOENGINE_VULKANDESCRIPTORSETLAYOUT_H