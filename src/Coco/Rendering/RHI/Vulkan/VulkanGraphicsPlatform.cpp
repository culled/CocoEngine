//
// Created by cullen on 3/22/26.
//

#include "VulkanGraphicsPlatform.h"

#include "Coco/Core/Engine.h"

#include "Coco/Core/Application.h"
#include "Coco/Rendering/RenderService.h"
#include "VulkanRenderingExtensions.h"
#include "Coco/Rendering/Graphics/Slang/SlangCompiler.h"
#include "Coco/Core/Types/Sorting/QSorter.h"
#include "Coco/Rendering/Graphics/GraphicsResourceCache.h"
#include "Resources/VulkanBuffer.h"
#include "Resources/VulkanGraphicsSurface.h"
#include "Resources/VulkanImage.h"
#include "Resources/VulkanImageSampler.h"
#include "Resources/VulkanRenderContext.h"
#include "Resources/VulkanShaderProgram.h"
#include "VulkanRenderFrame.h"
#include "VulkanStagingBuffer.h"
#include "VulkanUtils.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"


namespace Coco
{
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        const char* messageFormat = "Validation layer message:\n\t%s";

        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                COCO_ENGINE_LOG_ERROR(messageFormat, pCallbackData->pMessage);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                COCO_ENGINE_LOG_WARN(messageFormat, pCallbackData->pMessage);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            {
                COCO_ENGINE_LOG_VERBOSE(messageFormat, pCallbackData->pMessage);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            default:
                COCO_ENGINE_LOG_INFO(messageFormat, pCallbackData->pMessage);
                break;
        }

        return VK_FALSE;
    }

    VulkanGraphicsPlatformCreateParams::VulkanGraphicsPlatformCreateParams(const Application& app, const GraphicsDeviceCreateParams& deviceCreateParams) :
        App(&app),
        EnableDebugging(false),
        DeviceCreateParams(deviceCreateParams)
    {}

    VulkanGraphicsPlatform::VulkanGraphicsPlatform(RenderService* renderService, const VulkanGraphicsPlatformCreateParams& createParams) :
        GraphicsPlatform(renderService),
        _instance(nullptr),
        _platformAPIVersion(0),
        _debugMessenger(nullptr),
        _allocationCallbacks(),
        _physicalDevice(nullptr),
        _device(nullptr),
        _deviceAllocator(nullptr),
        _presentQueue(nullptr),
        _resourceManager(),
        _shaderProgramCompiler(),
        _graphicsResourceCache(),
        _renderFrames(nullptr, 2),
        _currentRenderFrameIndex(0),
        _currentFrameNumber(0)
    {
        InitializeVulkan(createParams);

        if (createParams.EnableDebugging)
            CreateDebugMessenger();

        _physicalDevice = PickPhysicalDevice(createParams.DeviceCreateParams);

        if (!_physicalDevice)
            throw Exception("No valid GPU device could be found");

        CreateDevice(createParams.DeviceCreateParams);

        _shaderProgramCompiler = CreateDefaultUnique<SlangCompiler>(SLANG_SPIRV, "spirv_1_5");
        _resourceManager = CreateDefaultUnique<GraphicsResourceManager>();
        _meshStorage = CreateDefaultUnique<MeshStorage>(this, 2);
        _vulkanResourceCache = CreateDefaultUnique<VulkanResourceCache>(this);
        _graphicsResourceCache = CreateDefaultUnique<GraphicsResourceCache>(this);

        for (uint8 i = 0; i < 2; ++i)
            _renderFrames.EmplaceBack(CreateDefaultManagedRef<VulkanRenderFrame>(this));

        _renderFrames.Front()->NewFrame();

        COCO_ENGINE_LOG_VERBOSE("Created VulkanGraphicsPlatform");
    }

    VulkanGraphicsPlatform::~VulkanGraphicsPlatform()
    {
        _renderFrames.Clear(true);
        _graphicsResourceCache.reset();
        _vulkanResourceCache.reset();
        _meshStorage.reset();
        _resourceManager.reset();
        _shaderProgramCompiler.reset();

        _graphicsQueue.reset();
        _transferQueue.reset();
        _computeQueue.reset();

        if (_deviceAllocator)
        {
            vmaDestroyAllocator(_deviceAllocator);
            _deviceAllocator = nullptr;
        }

        if (_device)
        {
            vkDestroyDevice(_device, _allocationCallbacks.get());
            _device = nullptr;
        }

        if (_debugMessenger)
        {
            vkDestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, _allocationCallbacks.get());
            _debugMessenger = nullptr;
        }

        if (_instance)
        {
            vkDestroyInstance(_instance, _allocationCallbacks.get());
            _instance = nullptr;
        }

        volkFinalize();

        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanGraphicsPlatform");
    }

    Ref<RenderFrame> VulkanGraphicsPlatform::GetCurrentRenderFrame()
    {
        return Ref<VulkanRenderFrame>(_renderFrames[_currentRenderFrameIndex]);
    }

    void VulkanGraphicsPlatform::NextFrame()
    {
        _renderFrames[_currentRenderFrameIndex]->EndFrame();

        _currentRenderFrameIndex = (_currentRenderFrameIndex + 1) % _renderFrames.GetCount();
        ++_currentFrameNumber;

        _renderFrames[_currentRenderFrameIndex]->NewFrame();
        _meshStorage->SetCurrentDynamicMeshBuffer(_currentRenderFrameIndex);
    }

    Ref<RenderContext> VulkanGraphicsPlatform::CreateRenderContext()
    {
        return _resourceManager->Create<VulkanRenderContext>(this);
    }

    Ref<Image> VulkanGraphicsPlatform::CreateImage(const ImageDescription& imageDescription)
    {
        return _resourceManager->Create<VulkanImage>(this, imageDescription);
    }

    Ref<ImageSampler> VulkanGraphicsPlatform::CreateImageSampler(const ImageSamplerDescription& samplerDescription)
    {
        return _resourceManager->Create<VulkanImageSampler>(this, samplerDescription);
    }

    Ref<ShaderProgram> VulkanGraphicsPlatform::CreateShaderProgram(const FilePath& shaderPath)
    {
        return _resourceManager->Create<VulkanShaderProgram>(this, shaderPath);
    }

    Ref<Buffer> VulkanGraphicsPlatform::CreateBuffer(const BufferDescription& bufferDescription)
    {
        return _resourceManager->Create<VulkanBuffer>(this, bufferDescription);
    }

    StagingBuffer* VulkanGraphicsPlatform::GetStagingBuffer()
    {
        return &_renderFrames[_currentRenderFrameIndex]->GetStagingBuffer();
    }

    void VulkanGraphicsPlatform::InvalidateResource(uint64 resourceID)
    {
        _resourceManager->Invalidate(resourceID);
    }

    Ref<GraphicsSurface> VulkanGraphicsPlatform::CreateSurface(VkSurfaceKHR surface, const Sizei& framebufferSize)
    {
        return _resourceManager->Create<VulkanGraphicsSurface>(this, surface, framebufferSize);
    }

    VulkanQueue* VulkanGraphicsPlatform::GetQueue(VulkanQueue::Type type)
    {
        switch (type)
        {
            case VulkanQueue::Type::Graphics:
                return _graphicsQueue ? &_graphicsQueue.value() : nullptr;
            case VulkanQueue::Type::Transfer:
                return _transferQueue ? &_transferQueue.value() : nullptr;
            case VulkanQueue::Type::Compute:
                return _computeQueue ? &_computeQueue.value() : nullptr;
            default:
                COCO_ASSERT(false, "Invalid queue type");
                return nullptr;
        }
    }

    VulkanQueue* VulkanGraphicsPlatform::GetPresentQueue(VkSurfaceKHR surface)
    {
        if (_presentQueue)
            return _presentQueue;

        if (_graphicsQueue && CheckPresentQueueSupport(*_graphicsQueue, surface))
        {
            _presentQueue = &_graphicsQueue.value();
        }
        else if (_transferQueue && CheckPresentQueueSupport(*_transferQueue, surface))
        {
            _presentQueue = &_transferQueue.value();
        }
        else if (_computeQueue && CheckPresentQueueSupport(*_computeQueue, surface))
        {
            _presentQueue = &_computeQueue.value();
        }

        return _presentQueue;
    }

    void VulkanGraphicsPlatform::WaitForIdle()
    {
        vkDeviceWaitIdle(_device);
    }

    VkDebugUtilsMessengerCreateInfoEXT VulkanGraphicsPlatform::GetDebugMessengerCreateInfo()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = &VulkanDebugCallback;
        createInfo.pUserData = nullptr;

        return createInfo;
    }

    void VulkanGraphicsPlatform::CheckExtensionsSupport(ArrayContainer<const char*>& requiredExtensions)
    {
        uint32 extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        Array<VkExtensionProperties> supportedExtensions(extensionCount, VkExtensionProperties());
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.Data());

        for (const char* extension : requiredExtensions)
        {
            bool hasExtension = supportedExtensions.Contains([extension](const VkExtensionProperties& availableExtension)
            {
               return strcmp(extension, availableExtension.extensionName) == 0;
            });

            if (!hasExtension)
                throw Exception(FormatString("Platform does not support required extension \"%s\"", extension));
        }
    }

    void VulkanGraphicsPlatform::CheckLayersSupport(ArrayContainer<const char*>& requiredLayers)
    {
        uint32 layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        Array<VkLayerProperties> availableLayers(layerCount, VkLayerProperties());
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

        for (const char* layer : requiredLayers)
        {
            bool hasLayer = availableLayers.Contains([layer](const VkLayerProperties& availableLayer)
            {
               return strcmp(layer, availableLayer.layerName) == 0;
            });

            if (!hasLayer)
                throw Exception(FormatString("Platform does not support required layer \"%s\"", layer));
        }
    }

    DeviceQueueFamilyInfo VulkanGraphicsPlatform::GetDeviceQueueFamilyInfo(VkPhysicalDevice device)
    {
        uint32 queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        Array<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount, VkQueueFamilyProperties());
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.Data());

        DeviceQueueFamilyInfo queueFamilyInfo;
        int minTransferScore = std::numeric_limits<int>::max();

        for (uint32 i = 0; i < queueFamilyCount; i++)
        {
            const VkQueueFamilyProperties& familyProperties = queueFamilyProperties[i];
            int transferScore = 0;

            if (familyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                if (!queueFamilyInfo.GraphicsQueueFamilyIndex.has_value())
                    queueFamilyInfo.GraphicsQueueFamilyIndex = i;

                transferScore++;
            }

            if (familyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                // Prioritize compute queues without graphics queues
                if (!queueFamilyInfo.ComputeQueueFamilyIndex.has_value() || (familyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != VK_QUEUE_GRAPHICS_BIT)
                    queueFamilyInfo.ComputeQueueFamilyIndex = i;

                transferScore++;
            }

            if (familyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                // Pick the transfer queue with the lowest score,
                // that way it is likely it will not be shared with any other queues
                if (transferScore < minTransferScore)
                {
                    queueFamilyInfo.TransferQueueFamilyIndex = i;
                    minTransferScore = transferScore;
                }
            }
        }

        return queueFamilyInfo;
    }

    int VulkanGraphicsPlatform::CalculateDeviceScore(VkPhysicalDevice device,
        const GraphicsDeviceCreateParams& createParams)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        int score = 0;

        DeviceQueueFamilyInfo queueFamilyInfo = GetDeviceQueueFamilyInfo(device);
        if (!queueFamilyInfo.GraphicsQueueFamilyIndex.has_value())
            return -1;

        if (createParams.RequireComputeCapability && !queueFamilyInfo.ComputeQueueFamilyIndex.has_value())
            return -1;

        // Check device extensions
        uint32 extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        Array<VkExtensionProperties> deviceExtensions(extensionCount, VkExtensionProperties());
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceExtensions.Data());

        Array<const char*> requiredExtensions;

        if (createParams.SupportPresentation)
            requiredExtensions.Append(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        for (const char* requiredExtension : requiredExtensions)
        {
            if (!deviceExtensions.Contains(
                [requiredExtension](const VkExtensionProperties& properties)
                {
                    return strcmp(&properties.extensionName[0], requiredExtension) == 0;
                }
            ))
            {
                return -1;
            }
        }

        if (VulkanUtils::ToGraphicsDeviceType(deviceProperties.deviceType) == createParams.PreferredDeviceType)
        {
            score += 3;
        }
        else
        {
            switch (deviceProperties.deviceType)
            {
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    score += 2;
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    score += 1;
                    break;
                default:
                    break;
            }
        }

        return score;
    }

    void VulkanGraphicsPlatform::InitializeVulkan(const VulkanGraphicsPlatformCreateParams& createParams)
    {
        auto result = volkInitialize();
        if (result != VK_SUCCESS)
            throw Exception("Failed to find a valid Vulkan loader on the system");

        constexpr uint32 minVersion = VK_API_VERSION_1_3; // Support for SPIR-V 1.3 for Slang and validation layers
        vkEnumerateInstanceVersion(&_platformAPIVersion);

        if (_platformAPIVersion < minVersion)
            throw Exception(
                FormatString(
                    "Supported Vulkan version is less than the minimum required version %u.%u.%u",
                    VK_API_VERSION_MAJOR(minVersion),
                    VK_API_VERSION_MINOR(minVersion),
                    VK_API_VERSION_PATCH(minVersion)
                )
            );

        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.pEngineName = "Coco";
        appInfo.engineVersion = VulkanUtils::ToVkVersion(Engine::EngineVersion);
        appInfo.pApplicationName = createParams.App->GetName().CStr();
        appInfo.applicationVersion = VulkanUtils::ToVkVersion(createParams.App->GetVersion());
        appInfo.apiVersion = _platformAPIVersion;

        // TODO: headless rendering?
        VulkanRenderingExtensions extensions;
        extensions.InstanceExtensions.Append(VK_KHR_SURFACE_EXTENSION_NAME);

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        Array<const char*> instanceLayers;
        if (createParams.EnableDebugging)
        {
            extensions.InstanceExtensions.Append(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	        static const char* DebugValidationLayerName = "VK_LAYER_KHRONOS_validation";
            instanceLayers.Append(DebugValidationLayerName);
            debugCreateInfo = GetDebugMessengerCreateInfo();
        }

        _renderService->GetRenderingPlatform()->GetRenderingExtensions(*this, extensions);

        CheckExtensionsSupport(extensions.InstanceExtensions);
        CheckLayersSupport(instanceLayers);

        VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32>(extensions.InstanceExtensions.GetCount());
        createInfo.ppEnabledExtensionNames = extensions.InstanceExtensions.Data();
        createInfo.enabledLayerCount = static_cast<uint32>(instanceLayers.GetCount());
        createInfo.ppEnabledLayerNames = instanceLayers.Data();
        createInfo.pNext = createParams.EnableDebugging ? &debugCreateInfo : nullptr;

        AssertVkSuccess(vkCreateInstance(&createInfo, _allocationCallbacks.get(), &_instance));
        volkLoadInstance(_instance);

        COCO_ENGINE_LOG_INFO(
            "Created Vulkan instance using Vulkan API %u.%u.%u",
            VK_API_VERSION_MAJOR(_platformAPIVersion),
            VK_API_VERSION_MINOR(_platformAPIVersion),
            VK_API_VERSION_PATCH(_platformAPIVersion)
        );
    }

    void VulkanGraphicsPlatform::CreateDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = GetDebugMessengerCreateInfo();
        auto result = vkCreateDebugUtilsMessengerEXT(_instance, &createInfo, _allocationCallbacks.get(), &_debugMessenger);
        if (result != VK_SUCCESS)
        {
            COCO_ENGINE_LOG_ERROR("Failed to create Vulkan debug messenger");
            _debugMessenger = nullptr;
        }
    }

    VkPhysicalDevice VulkanGraphicsPlatform::PickPhysicalDevice(const GraphicsDeviceCreateParams& createParams)
    {
        uint32 deviceCount;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            return nullptr;

        Array<VkPhysicalDevice> devices(deviceCount, VkPhysicalDevice());
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.Data());

        int highestScoreIndex = -1;
        int highestScore = 0;
        for (uint64 i = 0; i < devices.GetCount(); ++i)
        {
            int score = CalculateDeviceScore(devices.At(i), createParams);

            if (score == -1)
                continue;

            if (score > highestScore)
            {
                highestScore = score;
                highestScoreIndex = static_cast<int>(i);
            }
        }

        return highestScoreIndex == -1 ? nullptr : devices[highestScoreIndex];
    }

    void VulkanGraphicsPlatform::CreateDevice(const GraphicsDeviceCreateParams& createParams)
    {
        // Fill out queue creation info
        DeviceQueueFamilyInfo queueFamilyInfo = GetDeviceQueueFamilyInfo(_physicalDevice);
        COCO_ASSERT(queueFamilyInfo.GraphicsQueueFamilyIndex, "GPU doesn't have a graphics queue");
        StackArray<int, 3> uniqueQueueFamilies{ *queueFamilyInfo.GraphicsQueueFamilyIndex };

        if (queueFamilyInfo.TransferQueueFamilyIndex &&
            uniqueQueueFamilies[0] != queueFamilyInfo.TransferQueueFamilyIndex.value())
        {
            uniqueQueueFamilies.EmplaceBack(*queueFamilyInfo.TransferQueueFamilyIndex);
        }

        if (queueFamilyInfo.ComputeQueueFamilyIndex)
        {
            bool conflicts = uniqueQueueFamilies.Contains(*queueFamilyInfo.ComputeQueueFamilyIndex);

            if (!conflicts)
                uniqueQueueFamilies.EmplaceBack(*queueFamilyInfo.ComputeQueueFamilyIndex);
        }

        // TODO: individual queue priorities?
        StackArray<VkDeviceQueueCreateInfo, 3> deviceQueueCreateInfos;
        const float queuePriority = 1.0f;
        for (const auto& familyIndex : uniqueQueueFamilies)
        {
            auto& createInfo = deviceQueueCreateInfos.EmplaceBack(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO);
            createInfo.queueFamilyIndex = familyIndex;
            createInfo.queueCount = 1;
            createInfo.pQueuePriorities = &queuePriority;
        }

        // Fill out device extensions
        Array<const char*> deviceExtensions;
        //deviceExtensions.Append(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        //deviceExtensions.Append(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

        if (createParams.SupportPresentation)
            deviceExtensions.Append(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        // TODO: selectable device features
        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(_physicalDevice, &deviceFeatures);

        _deviceDescription.SupportsWireframe = deviceFeatures.fillModeNonSolid;

        // Create the logical device
        VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        createInfo.pQueueCreateInfos = deviceQueueCreateInfos.Data();
        createInfo.queueCreateInfoCount = static_cast<uint32>(deviceQueueCreateInfos.GetCount());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.ppEnabledExtensionNames = deviceExtensions.Data();
        createInfo.enabledExtensionCount = static_cast<uint32>(deviceExtensions.GetCount());
        createInfo.enabledLayerCount = 0;

        // Enable dynamic rendering
        VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES};
        dynamicRenderingFeatures.dynamicRendering = true;

        VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES};
        timelineSemaphoreFeatures.timelineSemaphore = true;
        timelineSemaphoreFeatures.pNext = &dynamicRenderingFeatures;

        VkPhysicalDeviceSynchronization2Features synchronization2Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES};
        synchronization2Features.synchronization2 = true;
        synchronization2Features.pNext = &timelineSemaphoreFeatures;

        createInfo.pNext = &synchronization2Features;

        AssertVkSuccess(vkCreateDevice(_physicalDevice, &createInfo, _allocationCallbacks.get(), &_device));

        GetGPUDescription();

        // Create the device allocator
        VmaAllocatorCreateInfo allocatorCreateInfo{};
        // TODO: VMA crashes with buffer creation when trying to use a higher API version
        allocatorCreateInfo.vulkanApiVersion = _platformAPIVersion;
        allocatorCreateInfo.physicalDevice = _physicalDevice;
        allocatorCreateInfo.device = _device;
        allocatorCreateInfo.instance = _instance;
        allocatorCreateInfo.pAllocationCallbacks = _allocationCallbacks.get();

        // Since we're using Volk, we need to manually pass in the loaded Vulkan functions
        VmaVulkanFunctions vmaVulkanFunctions{};
        vmaVulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vmaVulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        vmaVulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
        vmaVulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        vmaVulkanFunctions.vkAllocateMemory = vkAllocateMemory;
        vmaVulkanFunctions.vkFreeMemory = vkFreeMemory;
        vmaVulkanFunctions.vkMapMemory = vkMapMemory;
        vmaVulkanFunctions.vkUnmapMemory = vkUnmapMemory;
        vmaVulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
        vmaVulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
        vmaVulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
        vmaVulkanFunctions.vkBindImageMemory = vkBindImageMemory;
        vmaVulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
        vmaVulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
        vmaVulkanFunctions.vkCreateBuffer = vkCreateBuffer;
        vmaVulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
        vmaVulkanFunctions.vkCreateImage = vkCreateImage;
        vmaVulkanFunctions.vkDestroyImage = vkDestroyImage;
        vmaVulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;
        vmaVulkanFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;
        vmaVulkanFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2;
        vmaVulkanFunctions.vkBindBufferMemory2KHR = vkBindBufferMemory2;
        vmaVulkanFunctions.vkBindImageMemory2KHR = vkBindImageMemory2;
        vmaVulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2;
        vmaVulkanFunctions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
        vmaVulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;

        allocatorCreateInfo.pVulkanFunctions = &vmaVulkanFunctions;

        AssertVkSuccess(vmaCreateAllocator(&allocatorCreateInfo, &_deviceAllocator));

        for (const auto& familyIndex : uniqueQueueFamilies)
        {
            VkQueue deviceQueue;
            vkGetDeviceQueue(_device, familyIndex, 0, &deviceQueue);

            if (queueFamilyInfo.GraphicsQueueFamilyIndex &&
                *queueFamilyInfo.GraphicsQueueFamilyIndex == familyIndex)
            {
                _graphicsQueue.emplace(this, VulkanQueue::Type::Graphics, deviceQueue, familyIndex);
            }

            if (queueFamilyInfo.TransferQueueFamilyIndex &&
                *queueFamilyInfo.TransferQueueFamilyIndex == familyIndex)
            {
                _transferQueue.emplace(this, VulkanQueue::Type::Transfer, deviceQueue, familyIndex);
            }

            if (queueFamilyInfo.ComputeQueueFamilyIndex &&
                *queueFamilyInfo.ComputeQueueFamilyIndex == familyIndex)
            {
                _computeQueue.emplace(this, VulkanQueue::Type::Compute, deviceQueue, familyIndex);
            }
        }

        COCO_ENGINE_LOG_INFO("Using Vulkan on device %s - driver version %s", _deviceDescription.Name.CStr(), ToString(_deviceDescription.DriverVersion).CStr());
    }

    void VulkanGraphicsPlatform::GetGPUDescription()
    {
        // Get basic device info
        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);
        _deviceDescription.Name = deviceProperties.deviceName;
        _deviceDescription.Type = VulkanUtils::ToGraphicsDeviceType(deviceProperties.deviceType);
        _deviceDescription.DriverVersion = VulkanUtils::ToVersion(deviceProperties.driverVersion);

        VkSampleCountFlags maxSamples = deviceProperties.limits.framebufferColorSampleCounts & deviceProperties.limits.framebufferDepthSampleCounts;
        _deviceDescription.MaximumMSAASamples = VulkanUtils::ToMSAASamples(maxSamples);

        _deviceDescription.MaxImageWidth = deviceProperties.limits.maxImageDimension1D;
        _deviceDescription.MaxImageHeight = deviceProperties.limits.maxImageDimension2D;
        _deviceDescription.MaxImageDepth = deviceProperties.limits.maxImageDimension3D;
        _deviceDescription.MinimumBufferAlignment = static_cast<uint32>(deviceProperties.limits.minUniformBufferOffsetAlignment);
        _deviceDescription.MaxAnisotropicLevel = static_cast<uint32>(deviceProperties.limits.maxSamplerAnisotropy);
        _deviceDescription.MaxPushConstantSize = deviceProperties.limits.maxPushConstantsSize;
    }

    bool VulkanGraphicsPlatform::CheckPresentQueueSupport(VulkanQueue& queue, VkSurfaceKHR surface)
    {
        VkBool32 supported = VK_FALSE;
        AssertVkSuccess(vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, queue.GetFamilyIndex(), surface, &supported));

        return supported == VK_TRUE;
    }
} // Coco