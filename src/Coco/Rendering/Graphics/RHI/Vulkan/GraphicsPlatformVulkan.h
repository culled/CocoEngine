#pragma once

#include <Coco/Rendering/Graphics/GraphicsPlatform.h>

#include "VulkanIncludes.h"
#include "GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
    /// @brief Vulkan implementation of a GraphicsPlatform
    class GraphicsPlatformVulkan final : public GraphicsPlatform
    {
        friend class ManagedRef<GraphicsPlatformVulkan>;
    public:
        /// @brief Transforms Coco view space (forward on Y, up on Z, right on X) to Vulkan/DirectX view space (forward on Z, up on -Y, right on X)
        static const Matrix4x4 CocoViewToRenderView;

    private:
        static const char* s_debugValidationLayerName;

        VkInstance _instance = nullptr;
        VkDebugUtilsMessengerEXT _debugMessenger = nullptr;

        bool _usingValidationLayers = false;

        GraphicsDeviceCreationParameters _deviceCreationParams;
        ManagedRef<GraphicsDeviceVulkan> _device;

    protected:
        GraphicsPlatformVulkan(const GraphicsPlatformCreationParameters& creationParams);

    public:
        ~GraphicsPlatformVulkan() final;

        Logging::Logger* GetLogger() noexcept final;
        RenderingRHI GetRHI() const noexcept final { return RenderingRHI::Vulkan; }
        GraphicsDevice* GetDevice() noexcept final;
        //void ResetDevice() final;
        Ref<GraphicsPresenter> CreatePresenter(const string& name) final;
        Ref<Buffer> CreateBuffer(const string& name, uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate) final;
        Ref<Image> CreateImage(const string& name, const ImageDescription& description) final;
        Ref<ImageSampler> CreateImageSampler(const string& name, const ImageSamplerProperties& properties) final;
        Matrix4x4 CreateOrthographicProjection(double left, double right, double bottom, double top, double nearClip, double farClip) noexcept final;
        Matrix4x4 CreateOrthographicProjection(double size, double aspectRatio, double nearClip, double farClip) noexcept final;
        Matrix4x4 CreatePerspectiveProjection(double fieldOfViewRadians, double aspectRatio, double nearClip, double farClip) noexcept final;

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

