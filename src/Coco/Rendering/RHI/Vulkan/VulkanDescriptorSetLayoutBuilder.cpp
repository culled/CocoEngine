//
// Created by cullen on 3/30/26.
//

#include "VulkanDescriptorSetLayoutBuilder.h"

#include "VulkanGraphicsPlatform.h"
#include "VulkanPipelineLayoutBuilder.h"
#include "VulkanUtils.h"

namespace Coco
{
    VulkanDescriptorSetLayoutBuilder::VulkanDescriptorSetLayoutBuilder(
        VulkanPipelineLayoutBuilder& pipelineLayoutBuilder) :
        _pipelineLayoutBuilder(&pipelineLayoutBuilder),
        _layout()
    {
        _layout.ShaderStages = VulkanUtils::ToShaderStageFlags(pipelineLayoutBuilder._currentShaderStage);
    }

    void VulkanDescriptorSetLayoutBuilder::AddDescriptorRanges(
        slang::TypeLayoutReflection* elementTypeLayout)
    {
        if (elementTypeLayout->getSize() > 0)
            AddAutomaticallyInducedUniformBuffer();

        AddRanges(elementTypeLayout);
    }

    VulkanDescriptorSetLayout VulkanDescriptorSetLayoutBuilder::FinishBuilding(VulkanGraphicsPlatform& platform)
    {
        /*if (!_layout.LayoutBindings.IsEmpty())
        {
            VkDescriptorSetLayoutCreateInfo createInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
            createInfo.flags = 0;
            createInfo.pBindings = _layout.LayoutBindings.Data();
            createInfo.bindingCount = static_cast<uint32>(_layout.LayoutBindings.GetCount());

            AssertVkSuccess(vkCreateDescriptorSetLayout(platform.GetDevice(), &createInfo, platform.GetAllocationCallbacks(), &_layout.DescriptorSetLayout));
        }*/

        return _layout;
    }

    void VulkanDescriptorSetLayoutBuilder::AddAutomaticallyInducedUniformBuffer()
    {
        uint32 bindingIndex = _layout.LayoutBindings.GetCount();

        VkDescriptorSetLayoutBinding& binding = _layout.LayoutBindings.EmplaceBack();
        binding.stageFlags = _pipelineLayoutBuilder->_currentShaderStage;
        binding.binding = bindingIndex;
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    void VulkanDescriptorSetLayoutBuilder::AddRanges(slang::TypeLayoutReflection* typeLayout)
    {
        int rangeCount = typeLayout->getDescriptorSetDescriptorRangeCount(0);
        for (int i = 0; i < rangeCount; i++)
        {
            AddDescriptorRange(typeLayout, i);
        }

        _pipelineLayoutBuilder->AddSubObjectRanges(typeLayout);
    }

    void VulkanDescriptorSetLayoutBuilder::AddDescriptorRange(slang::TypeLayoutReflection* typeLayout, int rangeIndex)
    {
        auto bindingType = typeLayout->getDescriptorSetDescriptorRangeType(0, rangeIndex);

        if (bindingType == slang::BindingType::PushConstant)
            return;

        auto descriptorCount = typeLayout->getDescriptorSetDescriptorRangeDescriptorCount(0, rangeIndex);
        //uint32 bindingIndex = _layout.LayoutBindings.GetCount();
        uint32 bindingIndex = static_cast<uint32>(rangeIndex) + _layout.LayoutBindings.GetCount();

        VkDescriptorSetLayoutBinding& binding = _layout.LayoutBindings.EmplaceBack();
        binding.stageFlags = _pipelineLayoutBuilder->_currentShaderStage;
        binding.binding = bindingIndex;
        binding.descriptorCount = descriptorCount;
        binding.descriptorType = VulkanUtils::ToVkDescriptorType(bindingType);
    }
} // Coco