#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Rendering/Graphics/GraphicsPlatform.h>

#include "VulkanIncludes.h"
#include "GraphicsDeviceVulkan.h"

namespace Coco::Rendering
{
    class RenderingService;

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
        GraphicsDevice* GetDevice() const noexcept final { return _device.get(); }
        void ResetDevice() final;
        Managed<GraphicsPresenter> CreatePresenter() final;
        GraphicsResourceRef<Buffer> CreateBuffer(uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate) final;
        GraphicsResourceRef<Image> CreateImage(const ImageDescription& description) final;
        GraphicsResourceRef<ImageSampler> CreateImageSampler(FilterMode filterMode, RepeatMode repeatMode, uint maxAnisotropy) final;

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

