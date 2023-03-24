#pragma once

#include <Coco/Rendering/Graphics/GraphicsPlatform.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class GraphicsDeviceVulkan;

    /// @brief Vulkan implementation of a GraphicsPlatform
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
        //void ResetDevice() final;
        WeakManagedRef<GraphicsPresenter> CreatePresenter() final;
        WeakManagedRef<Buffer> CreateBuffer(uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate) final;
        WeakManagedRef<Image> CreateImage(const ImageDescription& description) final;
        WeakManagedRef<ImageSampler> CreateImageSampler(FilterMode filterMode, RepeatMode repeatMode, uint maxAnisotropy) final;

        /// @brief Gets the Vulkan instance
        /// @return The Vulkan instance
        VkInstance GetInstance() const noexcept { return _instance; }

    private:
        /// @brief Checks for validation layer support
        /// @return True if this Vulkan runtime supports validation layers
        static bool CheckValidationLayersSupport() noexcept;

        /// @brief Constructs a debug messenger create info
        /// @return A debug messenger create info
        VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo() const noexcept;

        /// @brief Creates a debug messenger for the Vulkan instance
        /// @return True if the messenger was created
        bool CreateDebugMessenger() noexcept;

        /// @brief Destroys the active debug messenger for the Vulkan instance
        void DestroyDebugMessenger() noexcept;
    };
}

