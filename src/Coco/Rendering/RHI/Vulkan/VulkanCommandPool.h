//
// Created by cullen on 4/3/26.
//

#ifndef COCOENGINE_VULKANCOMMANDPOOL_H
#define COCOENGINE_VULKANCOMMANDPOOL_H
#include "VulkanQueue.h"
#include "VulkanForwardDeclarations.h"

#include "Coco/Core/Types/Array.h"

namespace Coco
{
    class VulkanGraphicsPlatform;

    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(VulkanGraphicsPlatform* platform, VulkanQueue::Type queueType);
        ~VulkanCommandPool();

        VkCommandBuffer AllocateCommandBuffer();
        void Reset();

    private:
        VulkanGraphicsPlatform* _platform;
        VkCommandPool _commandPool;
        Array<VkCommandBuffer> _commandBuffers;
        uint64 _currentCommandBufferIndex;
    };
} // Coco

#endif //COCOENGINE_VULKANCOMMANDPOOL_H