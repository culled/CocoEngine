//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANGRAPHICSPLATFORM_H
#define COCOENGINE_VULKANGRAPHICSPLATFORM_H

#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Rendering/Graphics/GraphicsPlatform.h"
#include "Coco/Rendering/Graphics/GraphicsResourceManager.h"
#include "Coco/Core/Types/Optional.h"
#include "Coco/Rendering/Graphics/GraphicsPlatformTypes.h"
#include "VulkanForwardDeclarations.h"
#include "VulkanQueue.h"
#include "VulkanResourceCache.h"

namespace Coco
{
    class Application;
    class VulkanRenderFrame;
    class VulkanStagingBuffer;

    struct VulkanGraphicsPlatformCreateParams
    {
        const Application* App;
        bool EnableDebugging;
        GraphicsDeviceCreateParams DeviceCreateParams;

        VulkanGraphicsPlatformCreateParams(const Application& app, const GraphicsDeviceCreateParams& deviceCreateParams);
    };

    struct DeviceQueueFamilyInfo
    {
        Optional<int> GraphicsQueueFamilyIndex;
        Optional<int> ComputeQueueFamilyIndex;
        Optional<int> TransferQueueFamilyIndex;
    };

    class VulkanGraphicsPlatform : public GraphicsPlatform
    {
    public:
        static constexpr const char* Name = "Vulkan";

    public:
        VulkanGraphicsPlatform(RenderService* renderService, const VulkanGraphicsPlatformCreateParams& createParams);
        ~VulkanGraphicsPlatform();

        const char* GetName() const override { return Name; }
        Ref<RenderFrame> GetCurrentRenderFrame() override;
        uint64 GetCurrentFrameNumber() const override { return _currentFrameNumber; }
        void NextFrame() override;
        Ref<RenderContext> CreateRenderContext() override;
        Ref<Image> CreateImage(const ImageDescription& imageDescription) override;
        Ref<ImageSampler> CreateImageSampler(const ImageSamplerDescription& samplerDescription) override;
        Ref<ShaderProgram> CreateShaderProgram(const FilePath& shaderPath) override;
        Ref<Buffer> CreateBuffer(const BufferDescription& bufferDescription) override;
        MeshStorage* GetMeshStorage() override { return _meshStorage.get(); }
        StagingBuffer* GetStagingBuffer() override;
        SlangCompiler* GetShaderProgramCompiler() override { return _shaderProgramCompiler.get(); }
        GraphicsResourceCache* GetResourceCache() override { return _graphicsResourceCache.get(); }
        void InvalidateResource(uint64 resourceID) override;

        VkInstance GetInstance() const { return _instance; }
        VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
        VkDevice GetDevice() const { return _device; }
        VkAllocationCallbacks* GetAllocationCallbacks() { return _allocationCallbacks.get(); }
        Ref<GraphicsSurface> CreateSurface(VkSurfaceKHR surface, const Sizei& framebufferSize);
        VulkanQueue* GetQueue(VulkanQueue::Type type);
        VulkanQueue* GetPresentQueue(VkSurfaceKHR surface);
        VmaAllocator GetVmaAllocator() const { return _deviceAllocator; }
        VulkanResourceCache* GetVulkanCache() { return _vulkanResourceCache.get(); }
        void WaitForIdle();

    private:
        VkInstance _instance;
        uint32 _platformAPIVersion;
        VkDebugUtilsMessengerEXT _debugMessenger;
        UniquePtr<VkAllocationCallbacks> _allocationCallbacks;
        VkPhysicalDevice _physicalDevice;
        VkDevice _device;
        VmaAllocator _deviceAllocator;
        Optional<VulkanQueue> _graphicsQueue;
        Optional<VulkanQueue> _transferQueue;
        Optional<VulkanQueue> _computeQueue;
        VulkanQueue* _presentQueue;

        UniquePtr<GraphicsResourceManager> _resourceManager;
        UniquePtr<MeshStorage> _meshStorage;
        UniquePtr<SlangCompiler> _shaderProgramCompiler;
        UniquePtr<VulkanResourceCache> _vulkanResourceCache;
        UniquePtr<GraphicsResourceCache> _graphicsResourceCache;
        Array<ManagedRef<VulkanRenderFrame>> _renderFrames;
        uint8 _currentRenderFrameIndex;
        uint64 _currentFrameNumber;

    private:
        static VkDebugUtilsMessengerCreateInfoEXT GetDebugMessengerCreateInfo();
        static void CheckExtensionsSupport(ArrayContainer<const char*>& requiredExtensions);
        static void CheckLayersSupport(ArrayContainer<const char*>& requiredLayers);
        static DeviceQueueFamilyInfo GetDeviceQueueFamilyInfo(VkPhysicalDevice device);
        static int CalculateDeviceScore(VkPhysicalDevice device, const GraphicsDeviceCreateParams& createParams);

        void InitializeVulkan(const VulkanGraphicsPlatformCreateParams& createParams);
        void CreateDebugMessenger();
        VkPhysicalDevice PickPhysicalDevice(const GraphicsDeviceCreateParams& createParams);
        void CreateDevice(const GraphicsDeviceCreateParams& createParams);
        void GetGPUDescription();

        bool CheckPresentQueueSupport(VulkanQueue& queue, VkSurfaceKHR surface);
    };
} // Coco

#endif //COCOENGINE_VULKANGRAPHICSPLATFORM_H