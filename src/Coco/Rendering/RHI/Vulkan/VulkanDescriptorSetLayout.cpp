//
// Created by cullen on 3/26/26.
//

#include "VulkanGraphicsPlatform.h"
#include "VulkanDescriptorSetLayout.h"

#include "VulkanUtils.h"

namespace Coco
{
    /*VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanGraphicsPlatform* platform) :
        _platform(platform),
        _layoutBindings(layoutBindings),
        _descriptorSetLayout(nullptr)
    {
        VkDescriptorSetLayoutCreateInfo createInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        createInfo.flags = 0;
        createInfo.pBindings = _layoutBindings.Data();
        createInfo.bindingCount = static_cast<uint32>(_layoutBindings.GetCount());

        AssertVkSuccess(vkCreateDescriptorSetLayout(_platform->GetDevice(), &createInfo, _platform->GetAllocationCallbacks(), &_descriptorSetLayout));
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
    {
        if (_descriptorSetLayout)
        {
            vkDestroyDescriptorSetLayout(_platform->GetDevice(), _descriptorSetLayout, _platform->GetAllocationCallbacks());
            _descriptorSetLayout = nullptr;
        }

        _layoutBindings.Clear(true);
    }

    Span<const VkDescriptorSetLayoutBinding> VulkanDescriptorSetLayout::GetLayoutBindings() const
    {
        return _layoutBindings;
    }*/
} // Coco