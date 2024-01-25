#include "Renderpch.h"
#include "VulkanPresenter.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsPlatform.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanImage.h"
#include "VulkanUtils.h"
#include "../../../RenderService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
    SwapchainSupportDetails::SwapchainSupportDetails() :
        SurfaceCapabilities{},
        SurfaceFormats(),
        PresentModes()
    {}

    VulkanPresenter::VulkanPresenter(const GraphicsResourceID& id, VulkanGraphicsDevice& device) :
        Presenter(id),
        _device(device),
        _vSyncMode(VSyncMode::EveryVBlank),
        _framebufferSize(),
        _swapchain(nullptr),
        _surface(nullptr),
        _backbufferDescription(),
        _backbuffers(),
        _currentBackbufferIndex(),
        _isSwapchainDirty(true),
        _handleFramesInFlightChanged(this, &VulkanPresenter::HandleFramesInFlightChanged)
    {
        RenderService& rendering = *RenderService::Get();
        _handleFramesInFlightChanged.Connect(rendering.OnMaxFramesInFlightChanged);

        CocoTrace("Created VulkanPresenter {}", ID)
    }

    VulkanPresenter::~VulkanPresenter()
    {
        _handleFramesInFlightChanged.DisconnectAll();

        DestroySwapchainObjects();

        if (_swapchain)
        {
            _device.WaitForIdle();

            vkDestroySwapchainKHR(_device.GetDevice(), _swapchain, _device.GetAllocationCallbacks());
            _swapchain = nullptr;
        }

        if (_surface)
        {
            _device.WaitForIdle();

            vkDestroySurfaceKHR(_device.GetVulkanInstance(), _surface, _device.GetAllocationCallbacks());
            _surface = nullptr;
        }

        CocoTrace("Destroyed VulkanPresenter {}", ID)
    }

    void VulkanPresenter::SetSurface(UniqueRef<PresenterSurface>&& surface)
    {
        if (const VulkanPresenterSurface* vulkanSurface = dynamic_cast<const VulkanPresenterSurface*>(surface.get()))
        {
            if (_surface)
            {
                CocoError("Surface is already initialized")
                vkDestroySurfaceKHR(_device.GetVulkanInstance(), vulkanSurface->Surface, _device.GetAllocationCallbacks());
                return;
            }

            _surface = vulkanSurface->Surface;
        }
        else
        {
            throw InvalidOperationException("Surface was not a VulkanGraphicsPresenterSurface");
        }
    }

    void VulkanPresenter::SetFramebufferSize(const SizeInt& size)
    {
        if (size == _framebufferSize)
            return;

        _framebufferSize = size;
        MarkDirty();
    }

    void VulkanPresenter::SetVSyncMode(VSyncMode mode)
    {
        if (_vSyncMode == mode)
            return;

        _vSyncMode = mode;
        MarkDirty();
    }

    bool VulkanPresenter::AcquireImage(Ref<VulkanGraphicsSemaphore> imageAcquiredSemaphore, Ref<VulkanImage>& outImage)
    {
        if (_currentBackbufferIndex.has_value())
        {
            outImage = GetAcquiredImage();
            return true;
        }

        if (!EnsureSwapchain())
        {
            CocoError("Swapchain is not ready for rendering")
            return false;
        }

        uint32 imageIndex;
        VkResult result = vkAcquireNextImageKHR(
            _device.GetDevice(),
            _swapchain,
            Math::MaxValue<uint64_t>(),
            imageAcquiredSemaphore->GetSemaphore(),
            VK_NULL_HANDLE,
            &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            CocoTrace("Swapchain is out of date. Marking as dirty...")
            MarkDirty();
            return false;
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            CocoWarn("Swapchain is suboptimal")
            return false;
        }
        else if (result != VK_SUCCESS)
        {
            CocoError("Failed to acquire backbuffer: {}", string_VkResult(result))
            return false;
        }

        _currentBackbufferIndex = imageIndex;
        outImage = _backbuffers.at(imageIndex);

        return true;
    }

    Ref<VulkanImage> VulkanPresenter::GetAcquiredImage() const
    {
        CocoAssert(_currentBackbufferIndex.has_value(), "Presenter has not acquired an image")
        return _backbuffers.at(_currentBackbufferIndex.value());
    }

    bool VulkanPresenter::Present(Ref<VulkanGraphicsSemaphore> waitSemaphore)
    {
        CocoAssert(HasSurface(), "No presenter surface has been set")

        if (!_currentBackbufferIndex.has_value())
        {
            CocoError("Backbuffer was not acquired")
            return false;
        }

        VulkanQueue* presentQueue = _device.GetOrCreatePresentQueue(_surface);
        if (!presentQueue)
        {
            CocoError("Device does not support presenting")
            return false;
        }

        uint32 backbufferIndex = _currentBackbufferIndex.value();
        _currentBackbufferIndex.reset();

        VkSemaphore vulkanSemaphore = waitSemaphore->GetSemaphore();

        VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.pWaitSemaphores = &vulkanSemaphore;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pSwapchains = &_swapchain;
        presentInfo.swapchainCount = 1;
        presentInfo.pImageIndices = &backbufferIndex;

        VkResult result = vkQueuePresentKHR(presentQueue->GetQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            CocoTrace("Swapchain is not ideal ({}). Marking dirty...", string_VkResult(result))
            MarkDirty();
            return false;
        }
        else if (result != VK_SUCCESS)
        {
            CocoError("Failed to queue the image for presentation: {}", string_VkResult(result));
            return false;
        }

        return true;
    }

    void VulkanPresenter::MarkDirty()
    {
        _isSwapchainDirty = true;
    }

    SwapchainSupportDetails VulkanPresenter::GetSwapchainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
    {
        SwapchainSupportDetails details{};

        AssertVkSuccess(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.SurfaceCapabilities));

        uint32_t formatCount;
        AssertVkSuccess(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));
        details.SurfaceFormats.resize(formatCount);
        AssertVkSuccess(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.SurfaceFormats.data()));

        uint32_t presentModeCount;
        AssertVkSuccess(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));
        details.PresentModes.resize(presentModeCount);
        AssertVkSuccess(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data()));

        return details;
    }

    VkPresentModeKHR VulkanPresenter::PickPresentMode(VSyncMode preferredVSyncMode, const SwapchainSupportDetails& supportDetails)
    {
        const VkPresentModeKHR preferredPresentMode = ToVkPresentMode(preferredVSyncMode);

        auto it = std::find_if(supportDetails.PresentModes.begin(), supportDetails.PresentModes.end(),
            [preferredPresentMode](const VkPresentModeKHR& mode) { return mode == preferredPresentMode; });

        return it != supportDetails.PresentModes.end() ? *it : supportDetails.PresentModes[0];
    }

    VkSurfaceFormatKHR VulkanPresenter::PickSurfaceFormat(const SwapchainSupportDetails& supportDetails)
    {
        auto it = std::find_if(supportDetails.SurfaceFormats.begin(), supportDetails.SurfaceFormats.end(),
            [](const VkSurfaceFormatKHR& format)
            {
                // TODO: configuable?
                return ToImagePixelFormat(format.format) == ImagePixelFormat::RGBA8 &&
                    ToImageColorSpace(format.format) == ImageColorSpace::sRGB &&
                    ToImageColorSpace(format.colorSpace) == ImageColorSpace::sRGB;
            });

        if (it != supportDetails.SurfaceFormats.end())
            return *it;

        CocoWarn("No matching surface format found. Falling back to first supported format")
        return supportDetails.SurfaceFormats.front();
    }

    VkExtent2D VulkanPresenter::PickBackbufferExtent(const SizeInt& preferredSize, const SwapchainSupportDetails& supportDetails)
    {
        if (supportDetails.SurfaceCapabilities.currentExtent.width != Math::MaxValue<uint32_t>())
            return supportDetails.SurfaceCapabilities.currentExtent;

        VkExtent2D extent = {
            Math::Clamp(static_cast<uint32_t>(preferredSize.Width),
                supportDetails.SurfaceCapabilities.minImageExtent.width,
                supportDetails.SurfaceCapabilities.maxImageExtent.width),
            Math::Clamp(static_cast<uint32_t>(preferredSize.Height),
                supportDetails.SurfaceCapabilities.minImageExtent.height,
                supportDetails.SurfaceCapabilities.maxImageExtent.height)
        };

        return extent;
    }

    uint32 VulkanPresenter::PickBackbufferCount(uint32 preferredCount, const SwapchainSupportDetails& supportDetails)
    {
        uint32 imageCount = Math::Max(preferredCount, supportDetails.SurfaceCapabilities.minImageCount);

        if (supportDetails.SurfaceCapabilities.maxImageCount > 0)
            imageCount = Math::Min(imageCount, supportDetails.SurfaceCapabilities.maxImageCount);

        return imageCount;
    }

    bool VulkanPresenter::EnsureSwapchain()
    {
        if (_swapchain && !_isSwapchainDirty)
            return true;

        return RecreateSwapchain();
    }

    bool VulkanPresenter::RecreateSwapchain()
    {
        CocoAssert(_surface, "Surface was not set. Did you call SetSurface?")

        VulkanQueue* presentQueue = _device.GetOrCreatePresentQueue(_surface);
        CocoAssert(presentQueue, "Device does not support presentation")

        VulkanQueue* graphicsQueue = _device.GetQueue(VulkanQueueType::Graphics);
        CocoAssert(graphicsQueue, "Device does not graphics operations")

        SwapchainSupportDetails swapchainSupport = GetSwapchainSupportDetails(_device.GetPhysicalDevice(), _surface);
        CocoAssert(swapchainSupport.PresentModes.size() > 0, "Device does not support any present modes")
        CocoAssert(swapchainSupport.SurfaceFormats.size() > 0, "Device does not support any surface formats")

        // Wait until all graphics operations have finished
        presentQueue->WaitForIdle();
        graphicsQueue->WaitForIdle();

        DestroySwapchainObjects();

        VkPresentModeKHR presentMode = PickPresentMode(_vSyncMode, swapchainSupport);
        VkSurfaceFormatKHR surfaceFormat = PickSurfaceFormat(swapchainSupport);
        VkExtent2D extent = PickBackbufferExtent(_framebufferSize, swapchainSupport);

        // TODO: configure backbuffer count based on desired vsync mode
        //uint32 framebufferCount = RenderService::cGet()->GetMaxFramesInFlight() + 2;
        uint32 framebufferCount = RenderService::cGet()->GetMaxFramesInFlight() + 1;
        uint32 imageCount = PickBackbufferCount(framebufferCount, swapchainSupport);

        VkSwapchainKHR oldSwapchain = _swapchain;

        VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        createInfo.surface = _surface;
        createInfo.presentMode = presentMode;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.minImageCount = imageCount;
        createInfo.imageArrayLayers = 1; // TODO: 3D/VR support?
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = swapchainSupport.SurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO: transparent compositing?
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = oldSwapchain;

        std::vector<uint32> queueFamilyIndices;
        if (presentQueue->GetFamilyIndex() != graphicsQueue->GetFamilyIndex())
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;

            queueFamilyIndices.emplace_back(presentQueue->GetFamilyIndex());
            queueFamilyIndices.emplace_back(graphicsQueue->GetFamilyIndex());
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

            queueFamilyIndices.emplace_back(presentQueue->GetFamilyIndex());
        }

        createInfo.queueFamilyIndexCount = static_cast<uint32>(queueFamilyIndices.size());
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();

        VkResult result = vkCreateSwapchainKHR(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_swapchain);

        if (oldSwapchain)
            vkDestroySwapchainKHR(_device.GetDevice(), oldSwapchain, _device.GetAllocationCallbacks());

        if (result != VK_SUCCESS)
        {
            CocoError("Failed to create swapchain: {}", string_VkResult(result))
            _swapchain = nullptr;

            return false;
        }

        _framebufferSize = SizeInt(extent.width, extent.height);

        _backbufferDescription = ImageDescription::Create2D(
            _framebufferSize.Width, _framebufferSize.Height,
            ToImagePixelFormat(surfaceFormat.format),
            ToImageColorSpace(surfaceFormat.format),
            ImageUsageFlags::Presented | ImageUsageFlags::RenderTarget | ImageUsageFlags::TransferDestination | ImageUsageFlags::TransferSource,
            false
        );

        // Get backbuffer images
        uint32 backbufferImageCount;
        AssertVkSuccess(vkGetSwapchainImagesKHR(_device.GetDevice(), _swapchain, &backbufferImageCount, nullptr));
        std::vector<VkImage> images(backbufferImageCount);
        AssertVkSuccess(vkGetSwapchainImagesKHR(_device.GetDevice(), _swapchain, &backbufferImageCount, images.data()));

        // Use the current tick number to create unique IDs for each backbuffer
        uint64 s = MainLoop::cGet()->GetCurrentTick().TickNumber;

        for (size_t i = 0; i < images.size(); i++)
        {
            _backbuffers.emplace_back(
                CreateManagedRef<VulkanImage>(
                    Math::CombineHashes(i, s, ID),
                    _device,
                    _backbufferDescription,
                    images[i])
            );
        }

        _vSyncMode = ToVSyncMode(presentMode);
        _isSwapchainDirty = false;

        CocoTrace("Created Vulkan swapchain with {} backbuffers at {}", _backbuffers.size(), _framebufferSize.ToString())

        return true;
    }

    void VulkanPresenter::DestroySwapchainObjects()
    {
        _backbuffers.clear();

        _isSwapchainDirty = true;
    }

    bool VulkanPresenter::HandleFramesInFlightChanged(uint32 framesInFlight)
    {
        MarkDirty();

        return false;
    }
}