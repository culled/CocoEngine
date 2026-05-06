//
// Created by cullen on 3/26/26.
//

#include "VulkanGraphicsPlatform.h"

#include "VulkanDescriptorSetPool.h"
#include "VulkanUtils.h"

#include "Coco/Core/Engine.h"

namespace Coco
{
    VulkanDescriptorSetPool::VulkanDescriptorSetPool(VulkanGraphicsPlatform* platform,
        Ref<VulkanShaderProgram> shaderProgram) :
        _platform(platform),
        _shaderProgram(shaderProgram),
        _pools(nullptr, 1),
        _poolSizes(nullptr, 1),
        _lastAllocatedFrameNumber(0),
        _poolCreateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO}
    {
        Map<VkDescriptorType, uint32> typeCounts;
        auto setLayouts = _shaderProgram->GetDescriptorSetLayouts();

        for (const auto& setLayout : setLayouts)
        {
            for (const auto& layoutBinding : setLayout.LayoutBindings)
            {
                if (typeCounts.Contains(layoutBinding.descriptorType))
                {
                    typeCounts.Get(layoutBinding.descriptorType) += layoutBinding.descriptorCount;
                }
                else
                {
                    typeCounts.Emplace(layoutBinding.descriptorType, layoutBinding.descriptorCount);
                }
            }
        }

        for (const auto& pair : typeCounts)
        {
            VkDescriptorPoolSize& poolSize = _poolSizes.EmplaceBack();
            poolSize.type = pair.first;
            poolSize.descriptorCount = pair.second * _maxDescriptors;
        }

        _poolCreateInfo.pPoolSizes = _poolSizes.Data();
        _poolCreateInfo.poolSizeCount = static_cast<uint32>(_poolSizes.GetCount());
        _poolCreateInfo.maxSets = _maxSets;
    }

    VulkanDescriptorSetPool::~VulkanDescriptorSetPool()
    {
        for (auto& pool : _pools)
            vkDestroyDescriptorPool(_platform->GetDevice(), pool.Pool, _platform->GetAllocationCallbacks());

        _pools.Clear(true);
        _platform->InvalidateResource(_shaderProgram->GetID());
    }

    VkDescriptorSet VulkanDescriptorSetPool::AllocateDescriptorSet(uint64 layoutIndex)
    {
        auto setLayouts = _shaderProgram->GetDescriptorSetLayouts();
        COCO_ASSERT(layoutIndex < setLayouts.size(), "Invalid layout index");
        VkDescriptorSetLayout setLayout = setLayouts[layoutIndex].DescriptorSetLayout;

        _lastAllocatedFrameNumber = _platform->GetCurrentFrameNumber();

        VkDescriptorSet descriptorSet = nullptr;

        VkDescriptorSetAllocateInfo allocInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &setLayout;

        for (auto& pool : _pools)
        {
            allocInfo.descriptorPool = pool.Pool;
            auto result = vkAllocateDescriptorSets(_platform->GetDevice(), &allocInfo, &descriptorSet);

            if (result == VK_SUCCESS)
            {
                pool.LastAllocatedFrameNumber = _lastAllocatedFrameNumber;
                break;
            }

            if (result != VK_ERROR_OUT_OF_POOL_MEMORY)
            {
                AssertVkSuccess(result);
            }
        }

        if (!descriptorSet)
        {
            VulkanDescriptorSetPoolInfo& pool = _pools.EmplaceBack();
            AssertVkSuccess(vkCreateDescriptorPool(_platform->GetDevice(), &_poolCreateInfo, _platform->GetAllocationCallbacks(), &pool.Pool));

            allocInfo.descriptorPool = pool.Pool;
            AssertVkSuccess(vkAllocateDescriptorSets(_platform->GetDevice(), &allocInfo, &descriptorSet));

            pool.LastAllocatedFrameNumber = _lastAllocatedFrameNumber;
        }

        return descriptorSet;
    }

    void VulkanDescriptorSetPool::Reset()
    {
        for (auto& pool : _pools)
        {
            AssertVkSuccess(vkResetDescriptorPool(_platform->GetDevice(), pool.Pool, 0));
        }
    }
} // Coco