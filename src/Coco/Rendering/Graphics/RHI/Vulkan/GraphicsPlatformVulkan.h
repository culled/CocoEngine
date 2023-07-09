#pragma once

#include <Coco/Rendering/Graphics/GraphicsPlatform.h>

#include "VulkanIncludes.h"
#include "GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
    /// @brief Vulkan implementation of a GraphicsPlatform
    class GraphicsPlatformVulkan final : public GraphicsPlatform
    {
    private:
        static const char* s_debugValidationLayerName;

        VkInstance _instance = nullptr;
        VkDebugUtilsMessengerEXT _debugMessenger = nullptr;

        bool _usingValidationLayers = false;

        GraphicsDeviceCreationParameters _deviceCreationParams;
        ManagedRef<GraphicsDeviceVulkan> _device;

    public:
        GraphicsPlatformVulkan(const GraphicsPlatformCreationParameters& creationParams);
        ~GraphicsPlatformVulkan() final;

        Logging::Logger* GetLogger() noexcept final;
        RenderingRHI GetRHI() const noexcept final { return RenderingRHI::Vulkan; }
        GraphicsDevice* GetDevice() noexcept final;
        //void ResetDevice() final;
        Ref<GraphicsPresenter> CreatePresenter(const string& name) final;
        Ref<Buffer> CreateBuffer(const string& name, uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate) final;
        Ref<Image> CreateImage(const string& name, const ImageDescription& description) final;
        Ref<ImageSampler> CreateImageSampler(const string& name, const ImageSamplerProperties& properties) final;

        /// @brief Gets the Vulkan instance
        /// @return The Vulkan instance
        VkInstance GetInstance() noexcept { return _instance; }

    private:
        /// @brief Checks for validation layer support
        /// @return True if this Vulkan runtime supports validation layers
        static bool CheckValidationLayersSupport() noexcept;

        /// @brief Constructs a debug messenger create info
        /// @return A debug messenger create info
        VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo() noexcept;

        /// @brief Creates a debug messenger for the Vulkan instance
        /// @return True if the messenger was created
        bool CreateDebugMessenger() noexcept;

        /// @brief Destroys the active debug messenger for the Vulkan instance
        void DestroyDebugMessenger() noexcept;
    };
}

