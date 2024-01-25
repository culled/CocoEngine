#pragma once
#include "../../GraphicsPlatform.h"
#include <Coco/Core/Types/String.h>
#include "VulkanGraphicsPlatformFactory.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsPlatform :
        public GraphicsPlatform
    {
    public:
        static const string Name;

    public:
        VulkanGraphicsPlatform(const VulkanGraphicsPlatformCreateParams& createParams);
        ~VulkanGraphicsPlatform();

        // Inherited via GraphicsPlatform
        const string& GetName() const override { return Name; }
        Version GetAPIVersion() const override { return _apiVersion; }
        Matrix4x4 CreateOrthographicProjection(double left, double right, double bottom, double top, double nearClip, double farClip) override;
        Matrix4x4 CreateOrthographicProjection(double size, double aspectRatio, double nearClip, double farClip) override;
        Matrix4x4 CreatePerspectiveProjection(double verticalFOVRadians, double aspectRatio, double nearClip, double farClip) override;

        /// @brief Gets the Vulkan instance
        /// @return The Vulkan instance
        VkInstance GetVulkanInstance() const { return _instance; }

        /// @brief Gets the Vulkan allocation callbacks
        /// @return The Vulkan allocation callbacks
        const VkAllocationCallbacks* GetAllocationCallbacks() const { return nullptr; }

    protected:
        // Inherited via GraphicsPlatform
        UniqueRef<GraphicsDevice> CreateDevice() const override;

    private:
        VulkanGraphicsDeviceCreateParams _deviceCreateParams;
        Version _apiVersion;

        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debugMessenger;
        bool _usingValidationLayers;

    private:
        static VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo();
        static bool CheckValidationLayerSupport();
        static uint32 PickAPIVersion(const Version& version);

        void CreateVulkanInstance(const VulkanGraphicsPlatformCreateParams& createParams);
        void DestroyVulkanInstance();
        bool CreateDebugMessenger();
        void DestroyDebugMessenger();
    };
}