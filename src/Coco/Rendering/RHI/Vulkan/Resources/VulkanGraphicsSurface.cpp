//
// Created by cullen on 3/22/26.
//

#include "VulkanGraphicsSurface.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h"

#include "Coco/Core/Engine.h"

#include "VulkanGraphicsSemaphore.h"
#include "Coco/Rendering/RHI/Vulkan/Resources/VulkanImage.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanUtils.h"

#include <algorithm>

#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace Coco
{
    VulkanSwapchainResource::VulkanSwapchainResource(ManagedRef<VulkanImage>&& swapchainImage,
        ManagedRef<VulkanGraphicsSemaphore>&& renderingCompleteSemaphore) :
        SwapchainImage(std::move(swapchainImage)),
        ImageAcquiredSempahore(),
        RenderingCompleteSemaphore(std::move(renderingCompleteSemaphore))
    {}

    VulkanGraphicsSurface::VulkanGraphicsSurface(uint64 id, VulkanGraphicsPlatform* platform, VkSurfaceKHR surface, const Sizei& framebufferSize) :
        GraphicsSurface(id),
        _platform(platform),
        _surface(surface),
        _framebufferSize(framebufferSize),
        _vsyncMode(VSyncMode::EveryVBlank),
        _isDirty(true),
        _swapchain(nullptr),
        _swapchainResources(nullptr, 2),
        _acquiredSwapchainImageIndex(0)
    {
        COCO_ENGINE_LOG_VERBOSE("Created VulkanGraphicsSurface %u", id);
    }

    VulkanGraphicsSurface::~VulkanGraphicsSurface()
    {
        _platform->WaitForIdle();

        DestroySwapChain(_swapchain);

        if (_surface)
        {
            vkDestroySurfaceKHR(_platform->GetInstance(), _surface, _platform->GetAllocationCallbacks());
            _surface = nullptr;
        }

        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanGraphicsSurface %u", GetID());
    }

    void VulkanGraphicsSurface::SetFramebufferSize(const Sizei& framebufferSize)
    {
        if (_framebufferSize == framebufferSize)
            return;

        _framebufferSize = framebufferSize;
        MarkDirty();
    }

    void VulkanGraphicsSurface::SetVSyncMode(VSyncMode mode)
    {
        if (_vsyncMode == mode)
            return;

        _vsyncMode = mode;
        MarkDirty();
    }

    void VulkanGraphicsSurface::RebuildIfNeeded()
    {
        EnsureSwapChain();
    }

    bool VulkanGraphicsSurface::AcquireImage(Ref<VulkanGraphicsSemaphore> imageAcquiredSemaphore,
                                             Ref<VulkanImage>& outImage)
    {
        if (!EnsureSwapChain())
        {
            COCO_ENGINE_LOG_ERROR("Swapchain is not ready for rendering");
            return false;
        }

        VkResult result = vkAcquireNextImageKHR(
            _platform->GetDevice(),
            _swapchain,
            UINT64_MAX,
            imageAcquiredSemaphore->GetSemaphore(),
            nullptr,
            &_acquiredSwapchainImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            COCO_ENGINE_LOG_VERBOSE("Swapchain is out of date. Marking as dirty...");
            MarkDirty();
            return false;
        }

        if (result == VK_SUBOPTIMAL_KHR)
        {
            COCO_ENGINE_LOG_WARN("Swapchain is suboptimal");
        }
        else if (result != VK_SUCCESS)
        {
            COCO_ENGINE_LOG_ERROR("Failed to acquire backbuffer: %u", result);
            return false;
        }

        auto& swapchainResource = _swapchainResources[_acquiredSwapchainImageIndex];
        swapchainResource.ImageAcquiredSempahore = imageAcquiredSemaphore;
        outImage = swapchainResource.SwapchainImage;

        return true;
    }

    Ref<VulkanGraphicsSemaphore> VulkanGraphicsSurface::GetRenderingCompleteSemaphore()
    {
        return _swapchainResources[_acquiredSwapchainImageIndex].RenderingCompleteSemaphore;
    }

    void VulkanGraphicsSurface::Present()
    {
        VulkanQueue* presentQueue = _platform->GetPresentQueue(_surface);
        if (!presentQueue)
        {
            COCO_ENGINE_LOG_ERROR("Device does not support presentation");
            return;
        }

        VkSemaphore vulkanSemaphore = _swapchainResources[_acquiredSwapchainImageIndex].RenderingCompleteSemaphore->GetSemaphore();

        VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.pWaitSemaphores = &vulkanSemaphore;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pSwapchains = &_swapchain;
        presentInfo.swapchainCount = 1;
        presentInfo.pImageIndices = &_acquiredSwapchainImageIndex;

        VkResult result = vkQueuePresentKHR(presentQueue->GetQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            COCO_ENGINE_LOG_VERBOSE("Swapchain is not ideal: %u. Marking dirty...", result);
            MarkDirty();
        }
        else if (result != VK_SUCCESS)
        {
            COCO_ENGINE_LOG_ERROR("Failed to queue the image for presentation: %u", result);
        }
    }

    void VulkanGraphicsSurface::MarkDirty()
    {
        _isDirty = true;
    }

    bool VulkanGraphicsSurface::EnsureSwapChain()
    {
        if (!_isDirty)
            return true;

        return RecreateSwapChain();
    }

    VkSurfaceFormatKHR VulkanGraphicsSurface::PickSurfaceFormat() const
    {
        VkPhysicalDevice physicalDevice = _platform->GetPhysicalDevice();
        uint32 formatCount;
        AssertVkSuccess(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount, nullptr));

        if (formatCount == 0)
            return VkSurfaceFormatKHR(VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

        Array<VkSurfaceFormatKHR> surfaceFormats(formatCount, VkSurfaceFormatKHR());
        AssertVkSuccess(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount, surfaceFormats.Data()));

        uint64 bestRankIndex = 0;
        int highestScore = 0;
        for (uint64 i = 0; i < surfaceFormats.GetCount(); i++)
        {
            int score = 0;
            if (surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                score++;

            if (VulkanUtils::ToImagePixelFormat(surfaceFormats[i].format) == ImagePixelFormat::RGBA8)
                score += 2;

            if (score > highestScore)
            {
                highestScore = score;
                bestRankIndex = i;
            }
        }

        return surfaceFormats[bestRankIndex];
    }

    VkExtent2D VulkanGraphicsSurface::PickSurfaceExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const
    {
        // If this is set to the max value, then keep the currently set resolution
        if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return surfaceCapabilities.currentExtent;

        VkExtent2D extent = {
            Math::Clamp(static_cast<uint32_t>(_framebufferSize.Width),
                surfaceCapabilities.minImageExtent.width,
                surfaceCapabilities.maxImageExtent.width),
            Math::Clamp(static_cast<uint32_t>(_framebufferSize.Height),
                surfaceCapabilities.minImageExtent.height,
                surfaceCapabilities.maxImageExtent.height)
        };

        return extent;
    }

    uint32 VulkanGraphicsSurface::PickBackbufferCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const
    {
        uint32 imageCount = surfaceCapabilities.minImageCount + 1;

        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
            imageCount = surfaceCapabilities.maxImageCount;
        }

        return imageCount;
    }

    bool VulkanGraphicsSurface::RecreateSwapChain()
    {
        if (!_surface)
        {
            COCO_ENGINE_LOG_ERROR("Surface has been lost");
            return false;
        }

        VulkanQueue* graphicsQueue = _platform->GetQueue(VulkanQueue::Type::Graphics);
        if (!graphicsQueue)
        {
            COCO_ENGINE_LOG_ERROR("Device does not support graphics operations");
            return false;
        }

        VulkanQueue* presentQueue = _platform->GetPresentQueue(_surface);
        if (!presentQueue)
        {
            COCO_ENGINE_LOG_ERROR("Device does not support presenting");
            return false;
        }

        VkSurfaceFormatKHR surfaceFormat = PickSurfaceFormat();
        if (surfaceFormat.format == VK_FORMAT_UNDEFINED)
        {
            COCO_ENGINE_LOG_ERROR("Could not find a suitable swapchain surface format");
            return false;
        }

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        AssertVkSuccess(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_platform->GetPhysicalDevice(), _surface, &surfaceCapabilities));

        VkExtent2D surfaceExtent = PickSurfaceExtent(surfaceCapabilities);
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

        switch (_vsyncMode)
        {
            case VSyncMode::Immediate:
                presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                break;
            default: break;
        }

        uint32 imageCount = PickBackbufferCount(surfaceCapabilities);

        VkSwapchainKHR oldSwapchain = _swapchain;

        VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        createInfo.surface = _surface;
        createInfo.presentMode = presentMode;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = surfaceExtent;
        createInfo.minImageCount = imageCount;
        createInfo.imageArrayLayers = 1; // TODO: 3D/VR support?
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = surfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO: transparent compositing?
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = oldSwapchain;

        VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        StackArray<uint32, 2> queueFamilies = { presentQueue->GetFamilyIndex() };

        if (presentQueue->GetFamilyIndex() != graphicsQueue->GetFamilyIndex())
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            queueFamilies.Append(graphicsQueue->GetFamilyIndex());
        }

        createInfo.imageSharingMode = sharingMode;
        createInfo.pQueueFamilyIndices = queueFamilies.Data();
        createInfo.queueFamilyIndexCount = static_cast<uint32>(queueFamilies.GetCount());

        VkResult result = vkCreateSwapchainKHR(_platform->GetDevice(), &createInfo, _platform->GetAllocationCallbacks(), &_swapchain);
        DestroySwapChain(oldSwapchain);

        if (result != VK_SUCCESS)
        {
            COCO_ENGINE_LOG_ERROR("Failed to create swapchain: %u", result);
            return false;
        }

        _framebufferSize = Sizei(static_cast<int>(surfaceExtent.width), static_cast<int>(surfaceExtent.height));
        _vsyncMode = VulkanUtils::ToVSyncMode(presentMode);

        ImageDescription swapchainImageDescription = ImageDescription::Create2D(
            _framebufferSize.Width, _framebufferSize.Height,
            VulkanUtils::ToImagePixelFormat(surfaceFormat.format), VulkanUtils::ToImageColorSpace(surfaceFormat.format),
            ImageUsageFlags::RenderTarget | ImageUsageFlags::Presented | ImageUsageFlags::TransferDestination | ImageUsageFlags::TransferSource,
            false);

        uint32 swapchainImageCount = 0;
        AssertVkSuccess(vkGetSwapchainImagesKHR(_platform->GetDevice(), _swapchain, &swapchainImageCount, nullptr));
        Array<VkImage> swapchainImages(swapchainImageCount, nullptr);
        AssertVkSuccess(vkGetSwapchainImagesKHR(_platform->GetDevice(), _swapchain, &swapchainImageCount, swapchainImages.Data()));

        _swapchainResources.Reserve(swapchainImageCount);

        for (uint64 i = 0; i < swapchainImageCount; i++)
        {
            uint64 imageID = Math::CombineHashes(i, _platform->GetCurrentFrameNumber(), GetID());
            ManagedRef<VulkanImage> image = CreateDefaultManagedRef<VulkanImage>(imageID, _platform, swapchainImageDescription, swapchainImages[i]);
            ManagedRef<VulkanGraphicsSemaphore> renderingCompleteSemaphore = CreateDefaultManagedRef<VulkanGraphicsSemaphore>(imageID, _platform, false);
            _swapchainResources.EmplaceBack(std::move(image), std::move(renderingCompleteSemaphore));
        }

        COCO_ENGINE_LOG_VERBOSE("Created Vulkan swapchain with %u images at resolution %ux%u", swapchainImageCount, surfaceExtent.width, surfaceExtent.height);

        _isDirty = false;
        return true;
    }

    void VulkanGraphicsSurface::DestroySwapChain(VkSwapchainKHR& swapchain)
    {
        if (!swapchain)
            return;

        _platform->WaitForIdle();

        vkDestroySwapchainKHR(_platform->GetDevice(), swapchain, _platform->GetAllocationCallbacks());
        swapchain = nullptr;

        _swapchainResources.Clear();
    }
} // Coco