#pragma once

#include <Coco/Rendering/Graphics/GraphicsDevice.h>

#include <Coco/Core/Types/Optional.h>
#include "VulkanIncludes.h"
#include "CommandBufferPoolVulkan.h"
#include "VulkanRenderCache.h"

namespace Coco::Rendering::Vulkan
{
    class GraphicsPlatformVulkan;
    struct VulkanQueue;

    /// @brief A ranking for a VkPhysicalDevice
    struct PhysicalDeviceRanking
    {
        /// @brief The physical device
        VkPhysicalDevice Device;

        /// @brief The device's score
        int Score;

        PhysicalDeviceRanking(VkPhysicalDevice device, int score) noexcept;
    };

    /// @brief Queue families that can be used on a physical device
    struct PhysicalDeviceQueueFamilyInfo
    {
        Optional<int> GraphicsQueueFamily;
        Optional<int> TransferQueueFamily;
        Optional<int> ComputeQueueFamily;
    };

    /// @brief Vulkan implementation of a GraphicsDevice
    class GraphicsDeviceVulkan final : public GraphicsDevice
    {
    private:
        GraphicsPlatformVulkan* _platform = nullptr;

        VkPhysicalDevice _physicalDevice = nullptr;
        VkDevice _device = nullptr;

        string _name;
        GraphicsDeviceType _deviceType;
        Version _driverVersion;
        Version _apiVersion;
        GraphicsDeviceMemoryFeatures _memoryFeatures = {};
        uint _minUniformBufferAlignment;

        Optional<SharedRef<VulkanQueue>> _graphicsQueue;
        Optional<SharedRef<VulkanQueue>> _transferQueue;
        Optional<SharedRef<VulkanQueue>> _computeQueue;
        Optional<SharedRef<VulkanQueue>> _presentQueue;

        ManagedRef<VulkanRenderCache> _renderCache;
        Optional<ManagedRef<CommandBufferPoolVulkan>> _graphicsCommandPool;
        Optional<ManagedRef<CommandBufferPoolVulkan>> _transferCommandPool;
        Optional<ManagedRef<CommandBufferPoolVulkan>> _computeCommandPool;

    public:
        GraphicsDeviceVulkan(GraphicsPlatformVulkan& platform, VkPhysicalDevice physicalDevice, const GraphicsDeviceCreationParameters& createParams);
        ~GraphicsDeviceVulkan() final;

        Logging::Logger* GetLogger() noexcept final;
        string GetName() const noexcept final { return _name; }
        constexpr GraphicsDeviceType GetType() const noexcept final { return _deviceType; }
        Version GetDriverVersion() const noexcept final { return _driverVersion; }
        Version GetAPIVersion() const noexcept final { return _apiVersion; }
        const GraphicsDeviceMemoryFeatures& GetMemoryFeatures() const noexcept final { return _memoryFeatures; };
        constexpr uint GetMinimumBufferAlignment() const noexcept final { return _minUniformBufferAlignment; }
        void WaitForIdle() noexcept final;

        /// @brief Creates a graphics device with the given parameters
        /// @param platform The platform creating the device
        /// @param createParams The parameters for creating the device
        /// @return The created graphics device
        static ManagedRef<GraphicsDeviceVulkan> Create(GraphicsPlatformVulkan& platform, const GraphicsDeviceCreationParameters& createParams);

        GraphicsPlatformVulkan* GetVulkanPlatform() { return _platform; }

        /// @brief Gets the Vulkan physical device that this graphics device uses
        /// @return The Vulkan physical device
        VkPhysicalDevice GetPhysicalDevice() const noexcept { return _physicalDevice; }

        /// @brief Gets the Vulkan logical device that this graphics device uses
        /// @return The Vulkan logical device
        VkDevice GetDevice() const noexcept { return _device; }

        /// @brief Gets this device's Vulkan render cache
        /// @return This device's Vulkan render cache
        VulkanRenderCache* GetRenderCache() noexcept { return _renderCache.Get(); }

        /// @brief Attempts to initialize the present queue with the given surface
        /// @param surface The surface to use for initialization
        /// @return True if the present queue is initialized
        bool InitializePresentQueue(const VkSurfaceKHR& surface) noexcept;

        /// @brief Gets the graphics queue, if the device supports it
        /// @param graphicsQueue Will be filled out if this device has a graphics queue
        /// @return True if this device has a graphics queue
        bool GetGraphicsQueue(Ref<VulkanQueue>& graphicsQueue) const noexcept;

        /// @brief Gets the transfer queue, if the device supports it
        /// @param transferQueue Will be filled out if this device has a transfer queue
        /// @return True if this device has a transfer queue
        bool GetTransferQueue(Ref<VulkanQueue>& transferQueue) const noexcept;

        /// @brief Gets the compute queue, if the device supports it
        /// @param computeQueue Will be filled out if this device has a compute queue
        /// @return True if this device has a compute queue
        bool GetComputeQueue(Ref<VulkanQueue>& computeQueue) const noexcept;

        /// @brief Gets the present queue, if the device supports it
        /// @param presentQueue Will be filled out if this device has a present queue
        /// @return True if this device has a present queue
        bool GetPresentQueue(Ref<VulkanQueue>& presentQueue) const noexcept;

        /// @brief Gets this device's command pool for the graphics queue (if one has been created)
        /// @param poolPtr Will be filled out with the graphics command pool if this device has a graphics queue
        /// @return True if the graphics command pool was retrieved
        bool GetGraphicsCommandPool(Ref<CommandBufferPoolVulkan>& poolPtr) const noexcept;

        /// @brief Gets this device's command pool for the transfer queue (if one has been created)
        /// @param poolPtr Will be filled out with the transfer command pool if this device has a transfer queue
        /// @return True if the transfer command pool was retrieved
        bool GetTransferCommandPool(Ref<CommandBufferPoolVulkan>& poolPtr) const noexcept;

        /// @brief Gets this device's command pool for the compute queue (if one has been created)
        /// @param poolPtr Will be filled out with the compute command pool if this device has a compute queue
        /// @return True if the compute command pool was retrieved
        bool GetComputeCommandPool(Ref<CommandBufferPoolVulkan>& poolPtr) const noexcept;

        /// @brief Gets this device's command pool for the present queue (if one has been created)
        /// @param poolPtr Will be filled out with the present command pool if this device has a present queue
        /// @return True if the present command pool was retrieved
        bool GetPresentCommandPool(Ref<CommandBufferPoolVulkan>& poolPtr) const noexcept;

        /// @brief Finds a memory index for a type of memory
        /// @param type The type of memory
        /// @param memoryProperties The memory property flags
        /// @param memoryIndex Will be set to the index of the memory to use
        /// @return True if a valid memory index was found
        bool FindMemoryIndex(uint32_t type, VkMemoryPropertyFlags memoryProperties, uint32_t& memoryIndex) const noexcept;

        void PurgeUnusedResources() noexcept override;

    private:
        /// @brief Picks a suitable physical device to use given the parameters
        /// @param instance the Vulkan instance
        /// @param createParams Parameters for picking the physical device
        /// @return A suitable physical device
        static VkPhysicalDevice PickPhysicalDevice(const VkInstance& instance, const GraphicsDeviceCreationParameters& createParams);

        /// @brief Calculates a score for a physical device
        /// @param device The physical device
        /// @param createParams Parameters for picking the physical device
        /// @return A score for the device
        static int CalculateDeviceScore(const VkPhysicalDevice& device, const GraphicsDeviceCreationParameters& createParams) noexcept;

        /// @brief Gets the information about a device's queue families
        /// @param device The physical device
        /// @return Information about the device's queue families
        static PhysicalDeviceQueueFamilyInfo GetQueueFamilyInfo(const VkPhysicalDevice& device) noexcept;

        /// @brief Gets the memory features for a physical device
        /// @param device The physical device
        /// @return Memory features for the device
        static GraphicsDeviceMemoryFeatures GetDeviceMemoryFeatures(const VkPhysicalDevice& device) noexcept;

        /// @brief Checks if a queue supports surfaces
        /// @param surface The surface
        /// @param queue The queue to check for support
        /// @return True if the given queue supports presenting the given surface
        bool CheckQueuePresentSupport(const VkSurfaceKHR& surface, const Optional<Ref<VulkanQueue>>& queue) const noexcept;
    };
}

