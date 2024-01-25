#pragma once
#include "../../GraphicsResource.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    class VulkanGraphicsSemaphore :
        public GraphicsResource
    {
    public:
        VulkanGraphicsSemaphore(const GraphicsResourceID& id, VulkanGraphicsDevice& device);
        ~VulkanGraphicsSemaphore();

        VkSemaphore GetSemaphore() const { return _semaphore; }

    private:
        VulkanGraphicsDevice& _device;
        VkSemaphore _semaphore;
    };
}