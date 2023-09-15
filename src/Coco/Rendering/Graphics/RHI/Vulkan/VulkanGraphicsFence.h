#pragma once
#include "../../GraphicsFence.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief Vulkan implementation of a GraphicsFence
    class VulkanGraphicsFence : public GraphicsFence, public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        VkFence _fence;

    public:
        VulkanGraphicsFence(const GraphicsDeviceResourceID& id, bool startSignaled);
        ~VulkanGraphicsFence();

        bool IsSignaled() const final;
        void Reset() final;
        void Wait(uint64 timeoutNs) final;

        /// @brief Gets the Vulkan fence
        /// @return The Vulkan fence
        VkFence GetFence() const { return _fence; }
    };
}
