#pragma once
#include "../../../../Renderpch.h"
#include "../../../GraphicsResource.h"
#include "../VulkanCommandBuffer.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanQueue;

    class VulkanCommandBufferPool :
        public GraphicsResource
    {
    public:
        VulkanCommandBufferPool(const GraphicsResourceID& id, VulkanGraphicsDevice& device, const VulkanQueue& queue);
        ~VulkanCommandBufferPool();

        VulkanCommandBuffer Allocate(bool topLevel);
        void Free(const VulkanCommandBuffer& buffer);

    private:
        VulkanGraphicsDevice& _device;
        const VulkanQueue& _queue;
        VkCommandPool _pool;
        std::vector<VkCommandBuffer> _allocatedBuffers;
    };
}