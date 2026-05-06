//
// Created by cullen on 3/30/26.
//

#include "VulkanPipelineLayoutBuilder.h"

#include <csetjmp>

#include "VulkanDescriptorSetLayoutBuilder.h"
#include "VulkanGraphicsPlatform.h"
#include "VulkanUtils.h"

namespace Coco
{
    VulkanPipelineLayoutBuilder::VulkanPipelineLayoutBuilder(VulkanGraphicsPlatform* platform) :
        _platform(platform),
        _currentShaderStage(VK_SHADER_STAGE_ALL),
        _pipelineLayout()
    {}

    VulkanPipelineLayout VulkanPipelineLayoutBuilder::BuildForProgram(slang::ProgramLayout* programLayout)
    {
        for (uint8 entryPointIndex = 0; entryPointIndex < programLayout->getEntryPointCount(); entryPointIndex++)
        {
            auto entryPoint = programLayout->getEntryPointByIndex(entryPointIndex);
            switch (entryPoint->getStage())
            {
                case SLANG_STAGE_VERTEX:
                    _currentShaderStage = VK_SHADER_STAGE_VERTEX_BIT;
                    break;
                case SLANG_STAGE_FRAGMENT:
                    _currentShaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;
                case SLANG_STAGE_COMPUTE:
                    _currentShaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
                    break;
                case SLANG_STAGE_GEOMETRY:
                    _currentShaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
                    break;
                default:
                    // TODO: other stage types
                    continue;
            }

            AddDescriptorSetForParameterBlock(entryPoint->getTypeLayout());
        }

        _currentShaderStage = VK_SHADER_STAGE_ALL;
        auto globalTypeLayout = programLayout->getGlobalParamsVarLayout()->getTypeLayout();
        AddDescriptorSetForParameterBlock(globalTypeLayout);

        Array<VkDescriptorSetLayout> descriptorSetLayouts(nullptr, _pipelineLayout.DescriptorSetLayouts.GetCount());

        for (auto& layout : _pipelineLayout.DescriptorSetLayouts)
        {
            VkDescriptorSetLayoutCreateInfo createInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
            createInfo.flags = 0;
            createInfo.pBindings = layout.LayoutBindings.Data();
            createInfo.bindingCount = static_cast<uint32>(layout.LayoutBindings.GetCount());

            AssertVkSuccess(vkCreateDescriptorSetLayout(_platform->GetDevice(), &createInfo, _platform->GetAllocationCallbacks(), &layout.DescriptorSetLayout));
            descriptorSetLayouts.EmplaceBack(layout.DescriptorSetLayout);
        }

        VkPipelineLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        layoutInfo.pSetLayouts = descriptorSetLayouts.Data();
        layoutInfo.setLayoutCount = static_cast<uint32>(descriptorSetLayouts.GetCount());

        layoutInfo.pPushConstantRanges = _pipelineLayout.PushConstantRanges.Data();
        layoutInfo.pushConstantRangeCount = static_cast<uint32_t>( _pipelineLayout.PushConstantRanges.GetCount());

        AssertVkSuccess(vkCreatePipelineLayout(_platform->GetDevice(), &layoutInfo, _platform->GetAllocationCallbacks(), &_pipelineLayout.PipelineLayout));

        return _pipelineLayout;
    }

    void VulkanPipelineLayoutBuilder::AddDescriptorSetForParameterBlock(
        slang::TypeLayoutReflection* parameterBlockTypeLayout)
    {
        VulkanDescriptorSetLayoutBuilder layoutBuilder(*this);
        layoutBuilder.AddDescriptorRanges(parameterBlockTypeLayout);
        VulkanDescriptorSetLayout layout = layoutBuilder.FinishBuilding(*_platform);
        if (!layout.LayoutBindings.IsEmpty())
        {
            // TODO: better logic for detecting the same entrypoint descriptor layouts
            if (_currentShaderStage != VK_SHADER_STAGE_ALL)
            {
                for (auto& existingLayout : _pipelineLayout.DescriptorSetLayouts)
                {
                    if (existingLayout.LayoutBindings.GetCount() != layout.LayoutBindings.GetCount())
                    {
                        break;
                    }

                    bool isSame = true;

                    for (uint64 i = 0; i < existingLayout.LayoutBindings.GetCount(); i++)
                    {
                        const auto& a = existingLayout.LayoutBindings[i];
                        const auto& b = layout.LayoutBindings[i];

                        if (a.descriptorCount != b.descriptorCount ||
                            a.descriptorType != b.descriptorType)
                        {
                            isSame = false;
                            break;
                        }
                    }

                    if (!isSame)
                        break;

                    existingLayout.ShaderStages |= layout.ShaderStages;

                    for (auto& bindings : existingLayout.LayoutBindings)
                        bindings.stageFlags |= _currentShaderStage;

                    return;
                }

                _pipelineLayout.GlobalDescriptorSetIndexOffset++;
            }

            layout.DescriptorSetIndex = static_cast<uint32>(_pipelineLayout.DescriptorSetLayouts.GetCount());
            _pipelineLayout.DescriptorSetLayouts.EmplaceBack(layout);
        }
    }

    void VulkanPipelineLayoutBuilder::AddSubObjectRanges(slang::TypeLayoutReflection* typeLayout)
    {
        long subObjectRangeCount = typeLayout->getSubObjectRangeCount();

        for (long i = 0; i < subObjectRangeCount; i++)
        {
            AddSubObjectRange(typeLayout, i);
        }
    }

    void VulkanPipelineLayoutBuilder::AddSubObjectRange(slang::TypeLayoutReflection* typeLayout,
        long subObjectRangeIndex)
    {
        auto bindingRangeIndex = typeLayout->getSubObjectRangeBindingRangeIndex(subObjectRangeIndex);
        auto bindingType = typeLayout->getBindingRangeType(bindingRangeIndex);

        switch (bindingType)
        {
            case slang::BindingType::ParameterBlock:
            {
                auto parameterBlockTypeLayout = typeLayout->getBindingRangeLeafTypeLayout(bindingRangeIndex);
                AddDescriptorSetForParameterBlock(parameterBlockTypeLayout->getElementTypeLayout());
                break;
            }
            case slang::BindingType::PushConstant:
            {
                auto pushConstantTypeLayout = typeLayout->getBindingRangeLeafTypeLayout(bindingRangeIndex);
                AddPushConstantRange(pushConstantTypeLayout);
                break;
            }
            default:
                return;
        }
    }

    void VulkanPipelineLayoutBuilder::AddPushConstantRange(slang::TypeLayoutReflection* typeLayout)
    {
        auto elementTypeLayout = typeLayout->getElementTypeLayout();
        auto elementSize = elementTypeLayout->getSize();

        if (elementSize == 0)
            return;

        uint32 offset = 0;
        if (!_pipelineLayout.PushConstantRanges.IsEmpty())
        {
            // TODO: figure out way to determine actual push constant offsets from Slang
            auto& lastRange = _pipelineLayout.PushConstantRanges.Back();
            if (lastRange.size == elementSize)
            {
                lastRange.stageFlags |= _currentShaderStage;
                return;
            }

            offset = lastRange.offset + lastRange.size;
        }

        VkPushConstantRange& range = _pipelineLayout.PushConstantRanges.EmplaceBack();
        range.stageFlags = _currentShaderStage;
        range.offset = offset;
        range.size = elementSize;
    }
} // Coco