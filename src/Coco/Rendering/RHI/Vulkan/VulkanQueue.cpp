//
// Created by cullen on 3/22/26.
//

#include "VulkanQueue.h"

#include "volk.h"

namespace Coco
{
    VulkanQueue::VulkanQueue(VulkanGraphicsPlatform* platform, Type type, VkQueue queue, uint32 familyIndex) :
        _platform(platform),
        _type(type),
        _queue(queue),
        _queueFamilyIndex(familyIndex)
    {}

    void VulkanQueue::WaitForIdle()
    {
        vkQueueWaitIdle(_queue);
    }
} // Coco