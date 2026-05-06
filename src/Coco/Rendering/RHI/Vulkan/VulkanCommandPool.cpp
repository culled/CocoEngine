//
// Created by cullen on 4/3/26.
//

#include "VulkanCommandPool.h"

#include "VulkanGraphicsPlatform.h"
#include "VulkanIncludes.h"
#include "VulkanUtils.h"

namespace Coco
{
    VulkanCommandPool::VulkanCommandPool(VulkanGraphicsPlatform* platform, VulkanQueue::Type queueType) :
        _platform(platform),
        _commandPool(nullptr),
        _commandBuffers(nullptr, 3),
        _currentCommandBufferIndex(0)
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        commandPoolCreateInfo.queueFamilyIndex = _platform->GetQueue(queueType)->GetFamilyIndex();

        AssertVkSuccess(vkCreateCommandPool(_platform->GetDevice(), &commandPoolCreateInfo, _platform->GetAllocationCallbacks(), &_commandPool));
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        _commandBuffers.Clear(true);

        if (_commandPool)
        {
            _platform->WaitForIdle();
            vkDestroyCommandPool(_platform->GetDevice(), _commandPool, _platform->GetAllocationCallbacks());
            _commandPool = nullptr;
        }
    }

    VkCommandBuffer VulkanCommandPool::AllocateCommandBuffer()
    {
        if (_currentCommandBufferIndex < _commandBuffers.GetCount())
            return _commandBuffers[_currentCommandBufferIndex++];

        VkCommandBufferAllocateInfo allocateInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocateInfo.commandPool = _commandPool;
        allocateInfo.commandBufferCount = 1;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkCommandBuffer& commandBuffer = _commandBuffers.EmplaceBack(nullptr);
        AssertVkSuccess(vkAllocateCommandBuffers(_platform->GetDevice(), &allocateInfo, &commandBuffer));
        _currentCommandBufferIndex++;

        return commandBuffer;
    }

    void VulkanCommandPool::Reset()
    {
        vkResetCommandPool(_platform->GetDevice(), _commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        _currentCommandBufferIndex = 0;
    }
} // Coco