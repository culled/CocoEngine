#pragma once

#include <Coco/Rendering/Graphics/GraphicsPlatform.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class GraphicsDeviceVulkan;

    /// <summary>
    /// Vulkan implementation of a GraphicsPlatform
    /// </summary>
    class GraphicsPlatformVulkan final : public GraphicsPlatform
    {
    private:
        static const char* s_debugValidationLayerName;

        VkInstance _instance = nullptr;
        VkDebugUtilsMessengerEXT _debugMessenger = nullptr;

        bool _usingValidationLayers = false;

        GraphicsDeviceCreationParameters _deviceCreationParams;
        Managed<GraphicsDeviceVulkan> _device;

    public:
        GraphicsPlatformVulkan(Rendering::RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams);
        ~GraphicsPlatformVulkan() final;

        Logging::Logger* GetLogger() const noexcept final;
        RenderingRHI GetRHI() const noexcept final { return RenderingRHI::Vulkan; }
        GraphicsDevice* GetDevice() const noexcept final;
        void ResetDevice() final;
        WeakManagedRef<GraphicsPresenter> CreatePresenter() final;
        WeakManagedRef<Buffer> CreateBuffer(uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate) final;
        WeakManagedRef<Image> CreateImage(const ImageDescription& description) final;
        WeakManagedRef<ImageSampler> CreateImageSampler(FilterMode filterMode, RepeatMode repeatMode, uint maxAnisotropy) final;

        /// <summary>
        /// Gets the Vulkan instance
        /// </summary>
        /// <returns>The Vulkan instance</returns>
        VkInstance GetInstance() const noexcept { return _instance; }

    private:
        /// <summary>
        /// Checks for validation layer support
        /// </summary>
        /// <returns>True if this Vulkan runtime supports validation layers</returns>
        static bool CheckValidationLayersSupport() noexcept;

        /// <summary>
        /// Constructs a debug messenger create info
        /// </summary>
        /// <returns>A debug messenger create info</returns>
        VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo() const noexcept;

        /// <summary>
        /// Creates a debug messenger for the Vulkan instance
        /// </summary>
        /// <returns>True if the messenger was created</returns>
        bool CreateDebugMessenger() noexcept;

        /// <summary>
        /// Destroys the active debug messenger for the Vulkan instance
        /// </summary>
        void DestroyDebugMessenger() noexcept;
    };
}

