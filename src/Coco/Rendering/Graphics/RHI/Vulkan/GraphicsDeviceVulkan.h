#pragma once

#include <Coco/Rendering/Graphics/GraphicsDevice.h>

#include <Coco/Core/Types/Optional.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class GraphicsPlatformVulkan;
    struct VulkanQueue;
    class VulkanRenderCache;
    class CommandBufferPoolVulkan;

    /// <summary>
    /// A ranking for a VkPhysicalDevice
    /// </summary>
    struct PhysicalDeviceRanking
    {
        /// <summary>
        /// The physical device
        /// </summary>
        VkPhysicalDevice Device;

        /// <summary>
        /// The device's score
        /// </summary>
        int Score;

        PhysicalDeviceRanking(VkPhysicalDevice device, int score) noexcept;
    };

    /// <summary>
    /// Queue families that can be used on a physical device
    /// </summary>
    struct PhysicalDeviceQueueFamilyInfo
    {
        Optional<int> GraphicsQueueFamily;
        Optional<int> TransferQueueFamily;
        Optional<int> ComputeQueueFamily;
    };

    /// <summary>
    /// Vulkan implementation of a GraphicsDevice
    /// </summary>
    class GraphicsDeviceVulkan final : public GraphicsDevice
    {
    public:
        /// <summary>
        /// The platform that this device belongs to
        /// </summary>
        const GraphicsPlatformVulkan* VulkanPlatform;

    private:
        VkPhysicalDevice _physicalDevice = nullptr;
        VkDevice _device = nullptr;

        string _name;
        GraphicsDeviceType _deviceType;
        Version _driverVersion;
        Version _apiVersion;
        GraphicsDeviceMemoryFeatures _memoryFeatures = {};
        uint _minUniformBufferAlignment;

        Optional<Ref<VulkanQueue>> _graphicsQueue;
        Optional<Ref<VulkanQueue>> _transferQueue;
        Optional<Ref<VulkanQueue>> _computeQueue;
        Optional<Ref<VulkanQueue>> _presentQueue;

        Managed<VulkanRenderCache> _renderCache;
        Optional<Managed<CommandBufferPoolVulkan>> _graphicsCommandPool;
        Optional<Managed<CommandBufferPoolVulkan>> _transferCommandPool;
        Optional<Managed<CommandBufferPoolVulkan>> _computeCommandPool;

    public:
        GraphicsDeviceVulkan(const GraphicsPlatformVulkan& platform, VkPhysicalDevice physicalDevice, const GraphicsDeviceCreationParameters& createParams);
        ~GraphicsDeviceVulkan() final;

        Logging::Logger* GetLogger() const noexcept final;
        string GetName() const noexcept final { return _name; }
        GraphicsDeviceType GetType() const noexcept final { return _deviceType; }
        Version GetDriverVersion() const noexcept final { return _driverVersion; }
        Version GetAPIVersion() const noexcept final { return _apiVersion; }
        void WaitForIdle() noexcept final;
        const GraphicsDeviceMemoryFeatures& GetMemoryFeatures() const noexcept final { return _memoryFeatures; };
        uint GetMinimumBufferAlignment() const noexcept final { return _minUniformBufferAlignment; }

        /// <summary>
        /// Creates a graphics device with the given parameters
        /// </summary>
        /// <param name="platform">The platform creating the device</param>
        /// <param name="createParams">The parameters for creating the device</param>
        /// <returns>A created graphics device</returns>
        static Managed<GraphicsDeviceVulkan> Create(const GraphicsPlatformVulkan& platform, const GraphicsDeviceCreationParameters& createParams);

        /// <summary>
        /// Gets the Vulkan physical device that this graphics device uses
        /// </summary>
        /// <returns>The Vulkan physical device</returns>
        VkPhysicalDevice GetPhysicalDevice() const noexcept { return _physicalDevice; }

        /// <summary>
        /// Gets the Vulkan logical device that this graphics device uses
        /// </summary>
        /// <returns>The Vulkan logical device</returns>
        VkDevice GetDevice() const noexcept { return _device; }

        /// <summary>
        /// Gets this device's Vulkan render cache
        /// </summary>
        /// <returns>The Vulkan render cache</returns>
        VulkanRenderCache* GetRenderCache() const noexcept { return _renderCache.get(); }

        /// <summary>
        /// Attempts to initialize the present queue with the given surface
        /// </summary>
        /// <param name="surface">The surface to use for initialization</param>
        /// <returns>True if the present queue is initialized</returns>
        bool InitializePresentQueue(VkSurfaceKHR surface) noexcept;

        /// <summary>
        /// Gets the graphics queue, if the device supports it
        /// </summary>
        /// <param name="graphicsQueue">The queue reference that will be filled out if this device has a graphics queue</param>
        /// <returns>True if this device has a graphics queue</returns>
        bool GetGraphicsQueue(Ref<VulkanQueue>& graphicsQueue) const noexcept;

        /// <summary>
        /// Gets the transfer queue, if the device supports it
        /// </summary>
        /// <param name="transferQueue">The queue reference that will be filled out if this device has a transfer queue</param>
        /// <returns>True if this device has a transfer queue</returns>
        bool GetTransferQueue(Ref<VulkanQueue>& transferQueue) const noexcept;

        /// <summary>
        /// Gets the compute queue, if the device supports it
        /// </summary>
        /// <param name="computeQueue">The queue reference that will be filled out if this device has a compute queue</param>
        /// <returns>True if this device has a compute queue</returns>
        bool GetComputeQueue(Ref<VulkanQueue>& computeQueue) const noexcept;

        /// <summary>
        /// Gets the present queue, if the device supports it
        /// </summary>
        /// <param name="presentQueue">The queue reference that will be filled out if this device has a present queue</param>
        /// <returns>True if this device has a present queue</returns>
        bool GetPresentQueue(Ref<VulkanQueue>& presentQueue) const noexcept;

        /// <summary>
        /// Gets this device's command pool for the graphics queue (if a graphics queue has been created)
        /// </summary>
        /// <param name="poolPtr">A pointer reference that will be filled with a pointer to the pool if it exists</param>
        /// <returns>True if the graphics command pool exists</returns>
        bool GetGraphicsCommandPool(CommandBufferPoolVulkan*& poolPtr) const noexcept;

        /// <summary>
        /// Gets this device's command pool for the transfer queue (if a transfer queue has been created)
        /// </summary>
        /// <param name="poolPtr">A pointer reference that will be filled with a pointer to the pool if it exists</param>
        /// <returns>True if the transfer command pool exists</returns>
        bool GetTransferCommandPool(CommandBufferPoolVulkan*& poolPtr) const noexcept;

        /// <summary>
        /// Gets this device's command pool for the compute queue (if a compute queue has been created)
        /// </summary>
        /// <param name="poolPtr">A pointer reference that will be filled with a pointer to the pool if it exists</param>
        /// <returns>True if the compute command pool exists</returns>
        bool GetComputeCommandPool(CommandBufferPoolVulkan*& poolPtr) const noexcept;

        /// <summary>
        /// Gets this device's command pool for the present queue (if a present queue has been initialized)
        /// </summary>
        /// <param name="poolPtr">A pointer reference that will be filled with a pointer to the pool if it exists</param>
        /// <returns>True if the present command pool exists</returns>
        bool GetPresentCommandPool(CommandBufferPoolVulkan*& poolPtr) const noexcept;

        /// <summary>
        /// Finds a memory index for a type of memory
        /// </summary>
        /// <param name="type">The type of memory</param>
        /// <param name="memoryProperties">Memory properties</param>
        /// <param name="memoryIndex">The memory index</param>
        /// <returns>True if a valid memory index was found, or false if one could not be found</returns>
        bool FindMemoryIndex(uint32_t type, VkMemoryPropertyFlags memoryProperties, uint32_t& memoryIndex) const noexcept;

        /// <summary>
        /// Pads out an offset to align with this device's minimum uniform buffer alignment
        /// </summary>
        /// <param name="originalOffset">The original offset</param>
        /// <returns>An adjusted offset that respects this device's minimum uniform buffer alignment
        uint64_t GetPaddedUniformBufferAlignment(uint64_t originalOffset) const noexcept;

    protected:
        void OnPurgeUnusedResources() noexcept final;

    private:
        /// <summary>
        /// Picks a suitable physical device to use given the parameters
        /// </summary>
        /// <param name="instance">The Vulkan instance</param>
        /// <param name="createParams">Parameters for picking the device</param>
        /// <returns>The physical device that should be used</returns>
        static VkPhysicalDevice PickPhysicalDevice(VkInstance instance, const GraphicsDeviceCreationParameters& createParams);

        /// <summary>
        /// Calculates a score for a physical device
        /// </summary>
        /// <param name="device">The device</param>
        /// <param name="createParams">Parameters that the device should satisfy</param>
        /// <returns>The device's score, or -1 if the device doesn't meet a neccessary requirment</returns>
        static int CalculateDeviceScore(VkPhysicalDevice device, const GraphicsDeviceCreationParameters& createParams) noexcept;

        /// <summary>
        /// Gets the information about a device's queue families
        /// </summary>
        /// <param name="device">The device</param>
        /// <returns>Information about the device's queue families</returns>
        static PhysicalDeviceQueueFamilyInfo GetQueueFamilyInfo(VkPhysicalDevice device) noexcept;

        /// <summary>
        /// Gets the memory features for a physical device
        /// </summary>
        /// <param name="device">The physical device</param>
        /// <returns>The device's memory features</returns>
        static GraphicsDeviceMemoryFeatures GetDeviceMemoryFeatures(VkPhysicalDevice device) noexcept;

        /// <summary>
        /// Checks if a queue supports surfaces
        /// </summary>
        /// <param name="surface">The surface to check with</param>
        /// <param name="queue">The queue to check</param>
        /// <returns>True if the given queue supports surfaces</returns>
        bool CheckQueuePresentSupport(VkSurfaceKHR surface, const Optional<Ref<VulkanQueue>>& queue) const noexcept;
    };
}

