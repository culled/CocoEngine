#pragma once
#include "../../GraphicsResource.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    class VulkanGraphicsFence :
        public GraphicsResource
    {
    public:
        VulkanGraphicsFence(const GraphicsResourceID& id, VulkanGraphicsDevice& device, bool startSignaled);
        ~VulkanGraphicsFence();

        bool IsSignaled() const;
        void Reset();
        void WaitForSignal(uint64 timeoutNs);

        VkFence GetFence() const { return _fence; }

    private:
        VulkanGraphicsDevice& _device;
        VkFence _fence;
    };
}