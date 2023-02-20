#include "GraphicsPresenterVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include "GraphicsPlatformVulkan.h"
#include "VulkanUtilities.h"

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
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
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

	void GraphicsPresenterVulkan::CreateOrRecreateSwapchain()
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

		_isSwapchainDirty = false;
		_backbufferSize = SizeInt(static_cast<int>(extent.width), static_cast<int>(extent.height));
		_vsyncMode = ToVerticalSyncMode(presentMode);

		LogTrace(_device->VulkanPlatform->GetLogger(), 
			FormattedString("Created Vulkan swapchain with {} backbuffers at ({}, {})", imageCount, _backbufferSize.Width, _backbufferSize.Height));

		return true;
	}

	void GraphicsPresenterVulkan::DestroySwapchainObjects()
	{


		_isSwapchainDirty = true;
	}
}