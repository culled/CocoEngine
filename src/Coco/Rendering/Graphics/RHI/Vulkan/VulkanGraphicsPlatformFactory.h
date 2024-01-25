#pragma once
#include "../../GraphicsPlatformFactory.h"
#include "../../GraphicsDeviceTypes.h"
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Version.h>

namespace Coco
{
    class Application;
}

namespace Coco::Rendering::Vulkan
{
    struct VulkanGraphicsDeviceCreateParams
    {
        GraphicsDeviceType PreferredDeviceType;

        bool PresentationSupport;

        /// @brief If true, the device will be required to support compute operations
        bool RequireComputeCapability;

        /// @brief If true, the device will be required to support memory transfer operations
        bool RequireTransferCapability;

        /// @brief If true, the anisotropic sampling will be enabled on the device if it supports it
        bool EnableAnisotropicSampling;

        /// @brief If true, depth clamping will be enabled on the device if it supports it
        bool EnableDepthClamping;

        /// @brief If true, wireframe drawing will be enabled
        bool EnableWireframeDrawing;

        VulkanGraphicsDeviceCreateParams();
    };

    struct VulkanGraphicsPlatformCreateParams
    {
        static const Version DefaultAPIVersion;

        /// @brief The application instance.
        /// This is here because if the app creates the RenderService in its constructor, the Engine's app instance is technically not set yet and not accessible
        const Application& App;

        /// @brief The version of the Vulkan API to use
        Version APIVersion;

        /// @brief Set to true to enable the platform to present rendered images
        bool PresentationSupport;

        /// @brief If true, validation layers will be used, if supported
        bool UseValidationLayers;

        /// @brief Platform rendering extensions to enable
        std::vector<string> RenderingExtensions;

        VulkanGraphicsDeviceCreateParams DeviceCreateParams;

        VulkanGraphicsPlatformCreateParams(const Application& app, bool presentationSupport);
    };

    class VulkanGraphicsPlatformFactory :
        public GraphicsPlatformFactory
    {
    public:
        VulkanGraphicsPlatformFactory(const VulkanGraphicsPlatformCreateParams& createParams);

        // Inherited via GraphicsPlatformFactory
        UniqueRef<GraphicsPlatform> Create() const override;

    private:
        VulkanGraphicsPlatformCreateParams _createParams;
    };
}