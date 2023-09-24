#pragma once
#include "../../../GraphicsDeviceResource.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief A cached Vulkan resource
    class CachedVulkanResource :
        public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    protected:
        double _lastUseTime;

        CachedVulkanResource(const GraphicsDeviceResourceID& id);

    public:
        virtual ~CachedVulkanResource() = default;

        /// @brief Marks this resource as used
        void Use();

        /// @brief Determines if this resource is stale and can be purged
        /// @param staleThreshold The amount of time without use that a resource becomes stale
        /// @return True if this pass can be purged
        bool IsStale(double staleThreshold) const;
    };
}