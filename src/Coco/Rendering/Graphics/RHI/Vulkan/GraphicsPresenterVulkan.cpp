#include "GraphicsPresenterVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include "GraphicsPlatformVulkan.h"
#include "ImageVulkan.h"
#include "GraphicsFenceVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "VulkanUtilities.h"
#include "RenderContextVulkan.h"
#include "CommandBufferVulkan.h"
#include "BufferVulkan.h"

#include <vulkan/vk_enum_string_helper.h>

namespace Coco::Rendering
{
	GraphicsPresenterVulkan::GraphicsPresenterVulkan(GraphicsDeviceVulkan* device) :
		_device(device)
	{}

	GraphicsPresenterVulkan::~GraphicsPresenterVulkan()
	{
		DestroyRenderContexts();
		DestroySwapchainObjects();

		if (_swapchain != nullptr)
		{
			vkDestroySwapchainKHR(_device->GetDevice(), _swapchain, nullptr);
			_swapchain = nullptr;
		}

		if (_surface != nullptr)
		{
			vkDestroySurfaceKHR(_device->VulkanPlatform->GetInstance(), _surface, nullptr);
			_surface = nullptr;
		}

		LogTrace(_device->VulkanPlatform->GetLogger(), "Destroyed Vulkan presenter");
	}

	void GraphicsPresenterVulkan::InitializeSurface(PresenterSurfaceInitializationInfo* surfaceInitInfo)
	{
		if (_surface != nullptr)
			throw GraphicsPresenterException("No surface was given to initialize");

		if (PresenterWin32SurfaceInitializationInfo* win32SurfaceInitInfo = dynamic_cast<PresenterWin32SurfaceInitializationInfo*>(surfaceInitInfo))
		{
			VkWin32SurfaceCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.hwnd = static_cast<HWND>(win32SurfaceInitInfo->HWindow);
			createInfo.hinstance = static_cast<HINSTANCE>(win32SurfaceInitInfo->HInstance);

			AssertVkResult(vkCreateWin32SurfaceKHR(_device->VulkanPlatform->GetInstance(), &createInfo, nullptr, &_surface));

			_isSwapchainDirty = true;

			LogTrace(_device->VulkanPlatform->GetLogger(), "Created Vulkan surface");
		}
		else
		{
			throw GraphicsPresenterException("Unsupported surface type");
		}
	}

	void GraphicsPresenterVulkan::SetBackbufferSize(const SizeInt& backbufferSize) noexcept
	{
		if (backbufferSize == _backbufferSize)
			return;

		_backbufferSize = backbufferSize;
		_isSwapchainDirty = true;
	}

	void GraphicsPresenterVulkan::SetVSyncMode(VerticalSyncMode mode) noexcept
	{
		if (mode == _vsyncMode)
			return;

		_vsyncMode = mode;
		_isSwapchainDirty = true;
	}

	bool GraphicsPresenterVulkan::GetRenderContext(GraphicsResourceRef<RenderContext>& renderContext)
	{
		if (!EnsureSwapchainIsUpdated())
			return false;

		_currentFrame = (_currentFrame + 1) % static_cast<uint>(_backbuffers.Count());

		GraphicsResourceRef<RenderContextVulkan> currentContext = _renderContexts[_currentFrame];
		
		// Wait until a fresh frame is ready
		currentContext->WaitForRenderingCompleted();

		// TODO: figure out how to handle failed renders, as this will fail if an acquired image is not presented
		uint32_t imageIndex;
		const VkResult result = vkAcquireNextImageKHR(
			_device->GetDevice(),
			_swapchain,
			Math::MaxValue<uint64_t>(),
			currentContext->GetImageAvailableSemaphore(),
			VK_NULL_HANDLE,
			&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			_isSwapchainDirty = true;
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw VulkanOperationException(result, "Failed to acquire backbuffer image index: {}");

		currentContext->SetRenderTargets({ _backbuffers[imageIndex] });

		renderContext = currentContext;
		return true;
	}

	bool GraphicsPresenterVulkan::Present(RenderContext* renderContext)
	{
		Ref<VulkanQueue> presentQueue;

		if (!_device->GetPresentQueue(presentQueue))
			throw GraphicsPresenterException("Device must have a valid present queue to present");

		RenderContextVulkan* vulkanRenderContext = static_cast<RenderContextVulkan*>(renderContext);
		uint imageIndex = 0;
		bool imageFound = false;

		// TODO: just save the image index in the RenderContext?
		List<GraphicsResourceRef<ImageVulkan>> renderTargets = vulkanRenderContext->GetRenderTargets();
		for (int i = 0; i < _backbuffers.Count(); i++)
		{
			if (renderTargets.Contains(_backbuffers[i]))
			{
				imageIndex = i;
				imageFound = true;
				break;
			}
		}

		if (!imageFound)
		{
			LogError(_device->VulkanPlatform->GetLogger(), "Could not find backbuffer for render context");
			return false;
		}

		VkSemaphore waitSemaphore = vulkanRenderContext->GetRenderCompleteSemaphore();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pSwapchains = &_swapchain;
		presentInfo.swapchainCount = 1;
		presentInfo.pImageIndices = &imageIndex;

		const VkResult result = vkQueuePresentKHR(presentQueue->Queue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			_isSwapchainDirty = true;
			return false;
		}
		else if (result != VK_SUCCESS)
			throw VulkanOperationException(result, "Failed to queue the image for presentation: {}");

		return true;
	}

	VkPresentModeKHR GraphicsPresenterVulkan::PickPresentMode(VerticalSyncMode preferredVSyncMode, const SwapchainSupportDetails& supportDetails) noexcept
	{
		const VkPresentModeKHR preferredPresentMode = ToVkPresentMode(preferredVSyncMode);

		for (const VkPresentModeKHR& mode : supportDetails.PresentModes)
		{
			if (mode == preferredPresentMode)
				return mode;
		}

		return supportDetails.PresentModes[0];
	}

	VkSurfaceFormatKHR GraphicsPresenterVulkan::PickSurfaceFormat(const SwapchainSupportDetails& supportDetails) noexcept
	{
		for (const VkSurfaceFormatKHR& format : supportDetails.SurfaceFormats)
		{
			if (ToPixelFormat(format.format) == PixelFormat::RGBA8 && ToColorSpace(format.colorSpace) == ColorSpace::sRGB)
				return format;
		}

		return supportDetails.SurfaceFormats.First();
	}

	VkExtent2D GraphicsPresenterVulkan::PickBackbufferExtent(const SizeInt& preferredSize, const SwapchainSupportDetails& supportDetails) noexcept
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

	uint32_t GraphicsPresenterVulkan::PickBackbufferCount(int preferredCount, const SwapchainSupportDetails& supportDetails) noexcept
	{
		uint32_t imageCount = Math::Max(static_cast<uint32_t>(preferredCount), supportDetails.SurfaceCapabilities.minImageCount);

		if (supportDetails.SurfaceCapabilities.maxImageCount > 0)
			imageCount = Math::Min(imageCount, supportDetails.SurfaceCapabilities.maxImageCount);

		return imageCount;
	}

	SwapchainSupportDetails GraphicsPresenterVulkan::GetSwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapchainSupportDetails details = {};

		AssertVkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.SurfaceCapabilities));

		uint32_t formatCount;
		AssertVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));

		details.SurfaceFormats.Resize(formatCount);
		AssertVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.SurfaceFormats.Data()));

		uint32_t presentModeCount;
		AssertVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));

		details.PresentModes.Resize(presentModeCount);
		AssertVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.Data()));

		return details;
	}

	bool GraphicsPresenterVulkan::EnsureSwapchainIsUpdated()
	{
		// TODO: configurable frames in flight
		if (_isSwapchainDirty)
			return CreateSwapchain(_vsyncMode, _backbufferSize, _vsyncMode == VerticalSyncMode::Mailbox ? 3 : 2);
		else
			return true;
	}

	bool GraphicsPresenterVulkan::CreateSwapchain(VerticalSyncMode vsyncMode, const SizeInt& backbufferSize, int backbufferCount)
	{
		if (_surface == nullptr)
		{
			LogError(_device->VulkanPlatform->GetLogger(), "Window surface needs to be initialized before creating the swapchain");
			return false;
		}

		Ref<VulkanQueue> presentQueue;

		// If we can't get the present queue, try to initialize it and then get it.
		// If all that fails, we have no present queue
		if (!_device->GetPresentQueue(presentQueue) && (!_device->InitializePresentQueue(_surface) || !_device->GetPresentQueue(presentQueue)))
			throw FatalSwapchainInitializeException("Swapchain requires a device that supports presentation");

		Ref<VulkanQueue> graphicsQueue;
		if (!_device->GetGraphicsQueue(graphicsQueue))
			throw FatalSwapchainInitializeException("Swapchain requires a device that supports graphics operations");

		SwapchainSupportDetails swapchainSupportDetails = GetSwapchainSupportDetails(_device->GetPhysicalDevice(), _surface);

		if (swapchainSupportDetails.PresentModes.Count() == 0 || swapchainSupportDetails.SurfaceFormats.Count() == 0)
			throw FatalSwapchainInitializeException("Device has inadequate swapchain support");

		// Only recreate when all async work has finished
		_device->WaitForIdle();

		DestroySwapchainObjects();

		const VkPresentModeKHR presentMode = PickPresentMode(vsyncMode, swapchainSupportDetails);
		const VkSurfaceFormatKHR surfaceFormat = PickSurfaceFormat(swapchainSupportDetails);
		const VkExtent2D extent = PickBackbufferExtent(backbufferSize, swapchainSupportDetails);
		const uint32_t imageCount = PickBackbufferCount(backbufferCount, swapchainSupportDetails);

		VkSwapchainKHR oldSwapchain = _swapchain != nullptr ? _swapchain : VK_NULL_HANDLE;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
		createInfo.presentMode = presentMode;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.minImageCount = imageCount;
		createInfo.imageArrayLayers = 1; // TODO: 3D support?
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = swapchainSupportDetails.SurfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = oldSwapchain;

		List<uint32_t> queueFamilyIndices;

		if (presentQueue->QueueFamily != graphicsQueue->QueueFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;

			queueFamilyIndices.Add(static_cast<uint32_t>(presentQueue->QueueFamily));
			queueFamilyIndices.Add(static_cast<uint32_t>(graphicsQueue->QueueFamily));
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

			queueFamilyIndices.Add(static_cast<uint32_t>(presentQueue->QueueFamily));
		}

		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.Count());
		createInfo.pQueueFamilyIndices = queueFamilyIndices.Data();


		try
		{
			const VkResult result = vkCreateSwapchainKHR(_device->GetDevice(), &createInfo, nullptr, &_swapchain);

			if (oldSwapchain != VK_NULL_HANDLE)
				vkDestroySwapchainKHR(_device->GetDevice(), oldSwapchain, nullptr);

			if (result != VK_SUCCESS)
			{
				_swapchain = nullptr;
				throw VulkanOperationException(result);
			}

			_backbufferSize = SizeInt(static_cast<int>(extent.width), static_cast<int>(extent.height));
			_vsyncMode = ToVerticalSyncMode(presentMode);

			_backbufferDescription = ImageDescription(
				_backbufferSize.Width,
				_backbufferSize.Height,
				1,
				ToPixelFormat(surfaceFormat.format),
				ToColorSpace(surfaceFormat.colorSpace),
				ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled);

			// Get backbuffer images
			uint32_t backbufferImageCount;
			AssertVkResult(vkGetSwapchainImagesKHR(_device->GetDevice(), _swapchain, &backbufferImageCount, nullptr));

			List<VkImage> images(backbufferImageCount);
			AssertVkResult(vkGetSwapchainImagesKHR(_device->GetDevice(), _swapchain, &backbufferImageCount, images.Data()));

			for (int i = 0; i < images.Count(); i++)
			{
				_backbuffers.Add(_device->CreateResource<ImageVulkan>(_backbufferDescription, images[i]));
			}

			RecreateRenderContexts();

			_isSwapchainDirty = false;
			_currentFrame = 0;

			LogTrace(_device->VulkanPlatform->GetLogger(), FormattedString(
				"Created Vulkan swapchain with {} backbuffers at ({}, {})",
				imageCount,
				_backbufferSize.Width,
				_backbufferSize.Height));

			return true;
		}
		catch (const Exception& ex)
		{
			// Cleanup the malformed swapchain objects
			DestroyRenderContexts();
			DestroySwapchainObjects();

			if(_swapchain != nullptr)
				vkDestroySwapchainKHR(_device->GetDevice(), _swapchain, nullptr);

			_isSwapchainDirty = true;

			throw FatalSwapchainInitializeException(FormattedString("Failed to create swapchain: {}", ex.what()));
		}
	}

	void GraphicsPresenterVulkan::DestroySwapchainObjects() noexcept
	{
		try
		{
			for (GraphicsResourceRef<ImageVulkan>& backbuffer : _backbuffers)
			{
				backbuffer.reset();
			}

			_backbuffers.Clear();
		}
		catch(...)
		{ }

		_isSwapchainDirty = true;
	}

	void GraphicsPresenterVulkan::RecreateRenderContexts()
	{
		if (_renderContexts.Count() == _backbuffers.Count())
			return;

		DestroyRenderContexts();

		for (int i = 0; i < _backbuffers.Count(); i++)
		{
			_renderContexts.Add(_device->CreateResource<RenderContextVulkan>());
		}

		LogTrace(_device->VulkanPlatform->GetLogger(), FormattedString("Created {} render contexts", _renderContexts.Count()));
	}

	void GraphicsPresenterVulkan::DestroyRenderContexts() noexcept
	{
		_device->WaitForIdle();

		try
		{
			for (GraphicsResourceRef<RenderContextVulkan>& renderContext : _renderContexts)
			{
				renderContext.reset();
			}

			LogTrace(_device->VulkanPlatform->GetLogger(), FormattedString("Destroyed {} render contexts", _renderContexts.Count()));

			_renderContexts.Clear();
		}
		catch(...)
		{ }
	}
}