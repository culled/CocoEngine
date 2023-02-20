#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Rendering/Graphics/GraphicsDevice.h>
#include <Coco/Core/Types/Optional.h>

#include "VulkanQueue.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
    class GraphicsPlatformVulkan;

    struct PhysicalDeviceRanking
    {
        VkPhysicalDevice Device;
        int Score;

        PhysicalDeviceRanking(VkPhysicalDevice device, int score);
    };

    struct PhysicalDeviceQueueFamilyInfo
    {
        Optional<int> GraphicsQueueFamily;
        Optional<int> TransferQueueFamily;
        Optional<int> ComputeQueueFamily;
    };

    class GraphicsDeviceVulkan : public GraphicsDevice
    {
    public:
        const GraphicsPlatformVulkan* VulkanPlatform;

    private:
        VkPhysicalDevice _physicalDevice = nullptr;
        VkDevice _device = nullptr;

        string _name;
        GraphicsDeviceType _deviceType;
        Version _driverVersion;
        Version _apiVersion;

        Optional<Ref<VulkanQueue>> _graphicsQueue;
        Optional<Ref<VulkanQueue>> _transferQueue;
        Optional<Ref<VulkanQueue>> _computeQueue;
        Optional<Ref<VulkanQueue>> _presentQueue;

    public:
        GraphicsDeviceVulkan(GraphicsPlatformVulkan* platform, VkPhysicalDevice physicalDevice, const GraphicsDeviceCreationParameters& createParams);
        virtual ~GraphicsDeviceVulkan() override;

        static GraphicsDeviceVulkan* Create(GraphicsPlatformVulkan* platform, const GraphicsDeviceCreationParameters& createParams);

        virtual string GetName() const override { return _name; }
        virtual GraphicsDeviceType GetType() const override { return _deviceType; }
        virtual Version GetDriverVersion() const override { return _driverVersion; }
        virtual Version GetAPIVersion() const override { return _apiVersion; }

        VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
        VkDevice GetDevice() const { return _device; }

    private:
        static VkPhysicalDevice PickPhysicalDevice(VkInstance instance, const GraphicsDeviceCreationParameters& createParams);
        static int CalculateDeviceScore(VkPhysicalDevice device, const GraphicsDeviceCreationParameters& createParams);
        static PhysicalDeviceQueueFamilyInfo GetQueueFamilyInfo(VkPhysicalDevice device);
    };
}

