#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Rendering/Graphics/GraphicsPlatform.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
    class RenderingService;

    class GraphicsPlatformVulkan : public GraphicsPlatform
    {
    private:
        static const char* s_debugValidationLayerName;

        VkInstance _instance = nullptr;
        VkDebugUtilsMessengerEXT _debugMessenger = nullptr;

        bool _usingValidationLayers = false;

    public:
        GraphicsPlatformVulkan(Rendering::RenderingService* renderingService, const GraphicsBackendCreationParameters& creationParams);
        virtual ~GraphicsPlatformVulkan() override;

        virtual Logging::Logger* GetLogger() const override;
        virtual RenderingRHI GetRHI() const override { return RenderingRHI::Vulkan; }

    private:
        static bool CheckValidationLayersSupport();

        VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo() const;
        bool CreateDebugMessenger();
        void DestroyDebugMessenger();
    };
}

