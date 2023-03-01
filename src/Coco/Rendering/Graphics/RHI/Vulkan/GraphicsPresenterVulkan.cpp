#include "GraphicsPresenterVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include "GraphicsPlatformVulkan.h"
#include "ImageVulkan.h"
#include "GraphicsFenceVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "VulkanUtilities.h"
#include "RenderContextVulkan.h"
#include "CommandBufferVulkan.h"

namespace Coco::Rendering
{
	GraphicsPresenterVulkan::GraphicsPresenterVulkan(GraphicsDeviceVulkan* device) :
		_device(device)
	{
	}

	GraphicsPresenterVulkan::~GraphicsPresenterVulkan()
	{
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
			return;

		if (PresenterWin32SurfaceInitializationInfo* win32SurfaceInitInfo = dynamic_cast<PresenterWin32SurfaceInitializationInfo*>(surfaceInitInfo))
		{
			VkWin32SurfaceCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.hwnd = static_cast<HWND>(win32SurfaceInitInfo->HWindow);
			createInfo.hinstance = static_cast<HINSTANCE>(win32SurfaceInitInfo->HInstance);

			CheckVKResult(vkCreateWin32SurfaceKHR(_device->VulkanPlatform->GetInstance(), &createInfo, nullptr, &_surface));

			_isSwapchainDirty = true;

			LogTrace(_device->VulkanPlatform->GetLogger(), "Created Vulkan surface");
		}
		else
		{
			LogError(_device->VulkanPlatform->GetLogger(), "Unsupported surface type");
		}
	}

	void GraphicsPresenterVulkan::SetBackbufferSize(const SizeInt& backbufferSize)
	{
		if (backbufferSize == _backbufferSize)
			return;

		_backbufferSize = backbufferSize;
		_isSwapchainDirty = true;
	}

	void GraphicsPresenterVulkan::SetVSyncMode(VerticalSyncMode mode)
	{
		if (mode == _vsyncMode)
			return;

		_vsyncMode = mode;
		_isSwapchainDirty = true;
	}

	Managed<RenderContext> GraphicsPresenterVulkan::CreateRenderContext(const Ref<RenderView>& view, const Ref<RenderPipeline>& pipeline, int backbufferImageIndex)
	{
		EnsureSwapchainIsUpdated();

		// TODO: more than 1 attachment
		List<ImageVulkan*> images;
		images.Add(_backbuffers[backbufferImageIndex].Image);

		Managed<RenderContextVulkan> context = CreateManaged<RenderContextVulkan>(view, _device, pipeline, images, _commandBuffers[backbufferImageIndex]);

		// Add sync objects to the render context
		const BackBuffer& backbuffer = _backbuffers[backbufferImageIndex];
		context->AddWaitSemaphore(backbuffer.ImageAvailableSemaphore);
		context->AddSignalSemaphore(backbuffer.RenderingCompletedSemaphore);
		context->SetSignalFence(backbuffer.RenderingCompletedFence);

		// Ensure the framebuffers are up-to-date and valid for the render pipeline
		RecreateFramebuffers(pipeline, context->GetRenderPass());
		context->SetFramebuffer(_framebuffers[backbufferImageIndex]);

		return std::move(context);
	}

	GraphicsPresenterResult GraphicsPresenterVulkan::AcquireNextBackbuffer(
		unsigned long long timeoutNs,
		int& backbufferImageIndex)
	{
		EnsureSwapchainIsUpdated();

		const BackBuffer& lastBackbuffer = _backbuffers[_currentFrame];

		// Wait until a fresh frame is ready
		lastBackbuffer.RenderingCompletedFence->Wait(std::numeric_limits<unsigned long long>::max());
		lastBackbuffer.RenderingCompletedFence->Reset();

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(
			_device->GetDevice(), 
			_swapchain, 
			timeoutNs, 
			lastBackbuffer.ImageAvailableSemaphore->GetSemaphore(), 
			VK_NULL_HANDLE,
			&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			_isSwapchainDirty = true;
			return GraphicsPresenterResult::NeedsReinitialization;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Failed to acquire backbuffer image index: {}", string_VkResult(result)));
			return GraphicsPresenterResult::Failure;
		}

		backbufferImageIndex = static_cast<int>(imageIndex);
		_currentFrame = (_currentFrame + 1) % _backbuffers.Count();

		return GraphicsPresenterResult::Success;
	}

	GraphicsPresenterResult GraphicsPresenterVulkan::Present(int backbufferImageIndex)
	{
		if (!_device->GetPresentQueue().has_value())
		{
			LogError(_device->VulkanPlatform->GetLogger(), "Device must have a valid present queue");
			return GraphicsPresenterResult::Failure;
		}

		uint32_t imageIndex = static_cast<uint32_t>(backbufferImageIndex);

		VkSemaphore waitSemaphore = _backbuffers[backbufferImageIndex].RenderingCompletedSemaphore->GetSemaphore();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pSwapchains = &_swapchain;
		presentInfo.swapchainCount = 1;
		presentInfo.pImageIndices = &imageIndex;

		VkQueue presentQueue = _device->GetPresentQueue().value()->Queue;

		VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			_isSwapchainDirty = true;
			return GraphicsPresenterResult::NeedsReinitialization;
		}
		else if (result != VK_SUCCESS)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Failed to queue present: {}", string_VkResult(result)));
			return GraphicsPresenterResult::Failure;
		}

		return GraphicsPresenterResult::Success;
	}

	VkPresentModeKHR GraphicsPresenterVulkan::PickPresentMode(VerticalSyncMode preferredVSyncMode, const SwapchainSupportDetails& supportDetails)
	{
		VkPresentModeKHR preferredPresentMode = ToVkPresentMode(preferredVSyncMode);

		for (const VkPresentModeKHR& mode : supportDetails.PresentModes)
		{
			if (mode == preferredPresentMode)
				return mode;
		}

		return supportDetails.PresentModes[0];
	}

	VkSurfaceFormatKHR GraphicsPresenterVulkan::PickSurfaceFormat(const SwapchainSupportDetails& supportDetails)
	{
		for (const VkSurfaceFormatKHR& format : supportDetails.SurfaceFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		return supportDetails.SurfaceFormats[0];
	}

	VkExtent2D GraphicsPresenterVulkan::PickBackbufferExtent(const SizeInt& preferredSize, const SwapchainSupportDetails& supportDetails)
	{
		if (supportDetails.SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return supportDetails.SurfaceCapabilities.currentExtent;
		}

		VkExtent2D extent = { 
			std::clamp(static_cast<uint32_t>(preferredSize.Width), 
				supportDetails.SurfaceCapabilities.minImageExtent.width, 
				supportDetails.SurfaceCapabilities.maxImageExtent.width),
			std::clamp(static_cast<uint32_t>(preferredSize.Height),
				supportDetails.SurfaceCapabilities.minImageExtent.height,
				supportDetails.SurfaceCapabilities.maxImageExtent.height)
		};

		return extent;
	}

	uint32_t GraphicsPresenterVulkan::PickBackbufferCount(int preferredCount, const SwapchainSupportDetails& supportDetails)
	{
		uint32_t imageCount = std::max(static_cast<uint32_t>(preferredCount), supportDetails.SurfaceCapabilities.minImageCount);

		if (supportDetails.SurfaceCapabilities.maxImageCount > 0)
			imageCount = std::min(imageCount, supportDetails.SurfaceCapabilities.maxImageCount);

		return imageCount;
	}

	SwapchainSupportDetails GraphicsPresenterVulkan::GetSwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapchainSupportDetails details = {};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.SurfaceCapabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		details.SurfaceFormats.Resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.SurfaceFormats.Data());

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		details.PresentModes.Resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.Data());

		return details;
	}

	void GraphicsPresenterVulkan::EnsureSwapchainIsUpdated()
	{
		if(_isSwapchainDirty)
			CreateSwapchain(_vsyncMode, _backbufferSize, _vsyncMode == VerticalSyncMode::Mailbox ? 3 : 2);
	}

	bool GraphicsPresenterVulkan::CreateSwapchain(VerticalSyncMode vsyncMode, const SizeInt& backbufferSize, int backbufferCount)
	{
		if (_surface == nullptr)
		{
			LogError(_device->VulkanPlatform->GetLogger(), "Window surface needs to be initialized before creating the swapchain");
			return false;
		}

		if (!_device->GetPresentQueue().has_value() && !_device->InitializePresentQueue(_surface))
		{
			LogError(_device->VulkanPlatform->GetLogger(), "Device doesn't support presentation");
			return false;
		}

		if (!_device->GetGraphicsQueue().has_value())
		{
			LogError(_device->VulkanPlatform->GetLogger(), "Device doesn't support graphics operations");
			return false;
		}

		// Only recreate when all async work has finished
		_device->WaitForIdle();

		SwapchainSupportDetails swapchainSupportDetails = GetSwapchainSupportDetails(_device->GetPhysicalDevice(), _surface);

		if (swapchainSupportDetails.PresentModes.Count() == 0 || swapchainSupportDetails.SurfaceFormats.Count() == 0)
		{
			LogError(_device->VulkanPlatform->GetLogger(), "Device has inadequate swapchain support");
			return false;
		}

		DestroySwapchainObjects();

		VkPresentModeKHR presentMode = PickPresentMode(vsyncMode, swapchainSupportDetails);
		VkSurfaceFormatKHR surfaceFormat = PickSurfaceFormat(swapchainSupportDetails);
		VkExtent2D extent = PickBackbufferExtent(backbufferSize, swapchainSupportDetails);
		uint32_t imageCount = PickBackbufferCount(backbufferCount, swapchainSupportDetails);

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

		Ref<VulkanQueue> presentQueue = _device->GetPresentQueue().value();
		Ref<VulkanQueue> graphicsQueue = _device->GetGraphicsQueue().value();
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

		createInfo.queueFamilyIndexCount = queueFamilyIndices.Count();
		createInfo.pQueueFamilyIndices = queueFamilyIndices.Data();

		try
		{
			CheckVKResult(vkCreateSwapchainKHR(_device->GetDevice(), &createInfo, nullptr, &_swapchain));
		}
		catch (Exception& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Failed to create swapchain: {}", ex.what()));
			_swapchain = nullptr;
		}

		if(oldSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(_device->GetDevice(), oldSwapchain, nullptr);

		if (_swapchain == nullptr)
			return false;

		_backbufferSize = SizeInt(static_cast<int>(extent.width), static_cast<int>(extent.height));
		_vsyncMode = ToVerticalSyncMode(presentMode);

		_backbufferDescription = ImageDescription(
			_backbufferSize.Width, 
			_backbufferSize.Height, 
			1, 
			ToPixelFormat(surfaceFormat.format), 
			ToColorSpace(surfaceFormat.colorSpace));

		if (!GetBackbufferImages())
			return false;

		LogTrace(_device->VulkanPlatform->GetLogger(), FormattedString(
			"Created Vulkan swapchain with {} backbuffers at ({}, {})", 
			imageCount, 
			_backbufferSize.Width, 
			_backbufferSize.Height));

		_isSwapchainDirty = false;
		_currentFrame = 0;
		RecreateCommandBuffers();

		return true;
	}

	void GraphicsPresenterVulkan::DestroySwapchainObjects()
	{
		DestroyCommandBuffers();
		DestroyFramebuffers();

		for (const BackBuffer& backbuffer : _backbuffers)
		{
			_device->DestroyResource(backbuffer.Image);
			_device->DestroyResource(backbuffer.ImageAvailableSemaphore);
			_device->DestroyResource(backbuffer.RenderingCompletedSemaphore);
			_device->DestroyResource(backbuffer.RenderingCompletedFence);
		}

		_backbuffers.Clear();

		_isSwapchainDirty = true;
	}

	bool GraphicsPresenterVulkan::GetBackbufferImages()
	{
		try
		{
			uint32_t imageCount;
			CheckVKResult(vkGetSwapchainImagesKHR(_device->GetDevice(), _swapchain, &imageCount, nullptr));

			List<VkImage> images(imageCount);
			CheckVKResult(vkGetSwapchainImagesKHR(_device->GetDevice(), _swapchain, &imageCount, images.Data()));

			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.format = ToVkFormat(_backbufferDescription.PixelFormat);
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = static_cast<uint32_t>(_backbufferDescription.MipCount);
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = static_cast<uint32_t>(_backbufferDescription.Layers);

			for (int i = 0; i < images.Count(); i++)
			{
				createInfo.image = images[i];

				VkImageView view;
				CheckVKResult(vkCreateImageView(_device->GetDevice(), &createInfo, nullptr, &view));

				BackBuffer backbuffer = {};

				backbuffer.Image = new ImageVulkan(_device, _backbufferDescription, images[i], view, true);
				_device->AddResource(backbuffer.Image);

				backbuffer.ImageAvailableSemaphore = new GraphicsSemaphoreVulkan(_device);
				_device->AddResource(backbuffer.ImageAvailableSemaphore);

				backbuffer.RenderingCompletedSemaphore = new GraphicsSemaphoreVulkan(_device);
				_device->AddResource(backbuffer.RenderingCompletedSemaphore);

				backbuffer.RenderingCompletedFence = new GraphicsFenceVulkan(_device, true);
				_device->AddResource(backbuffer.RenderingCompletedFence);

				_backbuffers.Add(backbuffer);
			}

			return true;
		}
		catch (Exception& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Unable to get swapchain images: {}", ex.what()));
			return false;
		}
	}

	void GraphicsPresenterVulkan::RecreateFramebuffers(const Ref<RenderPipeline>& pipeline, VkRenderPass renderPass)
	{
		if (_framebuffers.Count() == _backbuffers.Count() && _framebufferPipeline.lock() == pipeline)
			return;

		DestroyFramebuffers();

		_framebufferPipeline = pipeline;

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.width = static_cast<uint32_t>(_backbufferSize.Width);
		framebufferInfo.height = static_cast<uint32_t>(_backbufferSize.Height);
		framebufferInfo.layers = 1;

		for (const BackBuffer& backbuffer : _backbuffers)
		{
			VkImageView view = backbuffer.Image->GetNativeView();
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &view;

			VkFramebuffer framebuffer;
			CheckVKResult(vkCreateFramebuffer(_device->GetDevice(), &framebufferInfo, nullptr, &framebuffer));

			_framebuffers.Add(framebuffer);
		}
	}

	void GraphicsPresenterVulkan::DestroyFramebuffers()
	{
		_device->WaitForIdle();

		for (const VkFramebuffer framebuffer : _framebuffers)
		{
			vkDestroyFramebuffer(_device->GetDevice(), framebuffer, nullptr);
		}

		LogTrace(_device->VulkanPlatform->GetLogger(), FormattedString("Destroyed {} framebuffers", _framebuffers.Count()));

		_framebuffers.Clear();
	}

	void GraphicsPresenterVulkan::RecreateCommandBuffers()
	{
		if (_commandBuffers.Count() == _backbuffers.Count())
			return;

		DestroyCommandBuffers();

		for (int i = 0; i < _backbuffers.Count(); i++)
		{
			_commandBuffers.Add(static_cast<CommandBufferVulkan*>(_device->GetGraphicsCommandPool().value()->Allocate(true)));
		}
	}

	void GraphicsPresenterVulkan::DestroyCommandBuffers()
	{
		_device->WaitForIdle();

		for (CommandBufferVulkan* buffer : _commandBuffers)
		{
			_device->GetGraphicsCommandPool().value()->Free(buffer);
		}

		LogTrace(_device->VulkanPlatform->GetLogger(), FormattedString("Destroyed {} command buffers", _commandBuffers.Count()));

		_commandBuffers.Clear();
	}
}