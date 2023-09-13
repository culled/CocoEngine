#pragma once

#include "../../GraphicsSemaphore.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief Vulkan implementation of a GraphicsSemaphore
    class VulkanGraphicsSemaphore : public GraphicsSemaphore, public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        VkSemaphore _semaphore;

    public:
        VulkanGraphicsSemaphore();
        ~VulkanGraphicsSemaphore();

        /// @brief Gets the Vulkan semaphore
        /// @return The Vulkan semaphore
        VkSemaphore GetSemaphore() const { return _semaphore; }
    };
}