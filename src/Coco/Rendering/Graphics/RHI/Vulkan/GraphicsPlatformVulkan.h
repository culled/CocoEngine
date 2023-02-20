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
    class GraphicsPlatformVulkan : public GraphicsPlatform
    {
    private:
        static const char* s_debugValidationLayerName;

        VkInstance _instance = nullptr;
        VkDebugUtilsMessengerEXT _debugMessenger = nullptr;

        bool _usingValidationLayers = false;

        Managed<GraphicsDeviceVulkan> _device;

    public:
        GraphicsPlatformVulkan(Rendering::RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams);
        virtual ~GraphicsPlatformVulkan() override;

        virtual Logging::Logger* GetLogger() const override;
        virtual RenderingRHI GetRHI() const override { return RenderingRHI::Vulkan; }
        virtual GraphicsDevice* GetDevice() const override { return _device.get(); }
        virtual GraphicsPresenter* CreatePresenter() override;

        /// <summary>
        /// Gets the Vulkan instance
        /// </summary>
        /// <returns>The Vulkan instance</returns>
        VkInstance GetInstance() const { return _instance; }

    private:
        /// <summary>
        /// Checks for validation layer support
        /// </summary>
        /// <returns>True if this Vulkan runtime supports validation layers</returns>
        static bool CheckValidationLayersSupport();

        /// <summary>
        /// Constructs a debug messenger create info
        /// </summary>
        /// <returns>A debug messenger create info</returns>
        VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo() const;

        /// <summary>
        /// Creates a debug messenger for the Vulkan instance
        /// </summary>
        /// <returns>True if the messenger was created</returns>
        bool CreateDebugMessenger();

        /// <summary>
        /// Destroys the active debug messenger for the Vulkan instance
        /// </summary>
        void DestroyDebugMessenger();
    };
}

