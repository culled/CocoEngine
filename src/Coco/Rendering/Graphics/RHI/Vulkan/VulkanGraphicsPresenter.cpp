#include "Renderpch.h"
#include "VulkanGraphicsPresenter.h"

#include "VulkanGraphicsPresenterSurface.h"
#include "VulkanUtils.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	SwapchainSupportDetails::SwapchainSupportDetails() :
		SurfaceCapabilities{},
		SurfaceFormats{},
		PresentModes{}
	{}

	const ImageUsageFlags VulkanGraphicsPresenter::_sBackbufferUsageFlags = ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled | ImageUsageFlags::Presented;

	VulkanGraphicsPresenter::VulkanGraphicsPresenter(const GraphicsDeviceResourceID& id) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_isSwapchainDirty(true),
		_framebufferSize(SizeInt::Zero),
		_vSyncMode(VSyncMode::EveryVBlank),
		_currentContextIndex(0),
		_backbufferDescription{},
		_renderContexts{},
		_backbuffers{},
		_acquiredBackbufferIndices{},
		_swapchain(nullptr),
		_surface(nullptr)
	{
		CocoTrace("Create VulkanGraphicsPresenter")
	}

	VulkanGraphicsPresenter::~VulkanGraphicsPresenter()
	{
		_device->WaitForIdle();

		DestroyRenderContexts();
		DestroySwapchainObjects();

		if (_swapchain)
		{
			vkDestroySwapchainKHR(_device->GetDevice(), _swapchain, _device->GetAllocationCallbacks());
			_swapchain = nullptr;
		}

		if (_surface)
		{
			vkDestroySurfaceKHR(_device->GetInstance(), _surface, _device->GetAllocationCallbacks());
			_surface = nullptr;
		}

		CocoTrace("Destroyed VulkanGraphicsPresenter")
	}

	void VulkanGraphicsPresenter::InitializeSurface(const GraphicsPresenterSurface& surface)
	{
		if (const VulkanGraphicsPresenterSurface* vulkanSurface = dynamic_cast<const VulkanGraphicsPresenterSurface*>(&surface))
		{
			if (SurfaceInitialized())
			{
				CocoError("Surface is already initialized")
				vkDestroySurfaceKHR(_device->GetInstance(), vulkanSurface->Surface, _device->GetAllocationCallbacks());
				return;
			}

			_surface = vulkanSurface->Surface;
		}
		else
		{
			throw std::exception("Surface was not a VulkanGraphicsPresenterSurface");
		}
	}

	bool VulkanGraphicsPresenter::PrepareForRender(Ref<RenderContext>& outContext, Ref<Image>& outBackbuffer)
	{
		if (!EnsureSwapchain())
		{
			CocoError("Swapchain is not ready for rendering")
			return false;
		}

		_currentContextIndex = (_currentContextIndex + 1) % static_cast<uint8>(_backbuffers.size());
		Ref<VulkanRenderContext> context = _renderContexts.at(_currentContextIndex);

		// Wait until the context is finished rendering
		context->WaitForRenderingToComplete();

		if (_acquiredBackbufferIndices.size() == _backbuffers.size())
		{
			CocoError("All backbuffers have been acquired. At least one must be released (i.e. presented) before rendering can begin")
			return false;
		}

		VkSemaphore imageAvailableSemaphore = static_cast<Ref<VulkanGraphicsSemaphore>>(context->GetRenderStartSemaphore())->GetSemaphore();

		uint32 imageIndex;
		VkResult result = vkAcquireNextImageKHR(
			_device->GetDevice(),
			_swapchain,
			Math::MaxValue<uint64_t>(),
			imageAvailableSemaphore,
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
		}
		else if (result != VK_SUCCESS)
		{
			CocoError("Failed to acquire backbuffer: {}", string_VkResult(result))
			return false;
		}

		context->Reset();

		context->SetBackbufferIndex(imageIndex);
		outContext = context;
		outBackbuffer = _backbuffers.at(imageIndex);
		_acquiredBackbufferIndices.emplace_back(imageIndex);

		return true;
	}

	bool VulkanGraphicsPresenter::Present(RenderContext& context)
	{
		DeviceQueue* presentQueue = _device->GetOrCreatePresentQueue(_surface);
		if (!presentQueue)
		{
			CocoError("Device does not support presenting")
			return false;
		}

		VulkanRenderContext* vulkanContext = static_cast<VulkanRenderContext*>(&context);

		int backbufferIndex = vulkanContext->GetBackbufferIndex();

		if (backbufferIndex == -1)
		{
			CocoError("The given RenderContext was not acquired from a GraphicsPresenter");
			return false;
		}

		uint32 imageIndex = static_cast<uint32>(backbufferIndex);

		const auto it = std::find(_acquiredBackbufferIndices.cbegin(), _acquiredBackbufferIndices.cend(), imageIndex);

		if (it == _acquiredBackbufferIndices.cend())
		{
			CocoError("The given RenderContext was not acquired from this GraphicsPresenter");
			return false;
		}

		_acquiredBackbufferIndices.erase(it);

		VkSemaphore waitSemaphore = static_cast<Ref<VulkanGraphicsSemaphore>>(vulkanContext->GetRenderCompletedSemaphore())->GetSemaphore();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pSwapchains = &_swapchain;
		presentInfo.swapchainCount = 1;
		presentInfo.pImageIndices = &imageIndex;

		VkResult result = vkQueuePresentKHR(presentQueue->Queue, &presentInfo);

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

	void VulkanGraphicsPresenter::SetVSync(VSyncMode mode)
	{
		if (mode == _vSyncMode)
			return;

		_vSyncMode = mode;
		MarkDirty();
	}

	void VulkanGraphicsPresenter::SetFramebufferSize(const SizeInt& size)
	{
		if (size == _framebufferSize)
			return;

		_framebufferSize = size;
		MarkDirty();
	}

	VkPresentModeKHR PickPresentMode(VSyncMode preferredVSyncMode, const SwapchainSupportDetails& supportDetails) noexcept
	{
		const VkPresentModeKHR preferredPresentMode = ToVkPresentMode(preferredVSyncMode);

		for (const VkPresentModeKHR& mode : supportDetails.PresentModes)
		{
			if (mode == preferredPresentMode)
				return mode;
		}

		return supportDetails.PresentModes[0];
	}

	VkSurfaceFormatKHR PickSurfaceFormat(const SwapchainSupportDetails& supportDetails) noexcept
	{
		for (const VkSurfaceFormatKHR& format : supportDetails.SurfaceFormats)
		{
			if (GetPixelFormat(format.format) == ImagePixelFormat::RGBA8 && ToImageColorSpace(format.colorSpace) == ImageColorSpace::sRGB)
				return format;
		}

		CocoWarn("No matching surface format found. Falling back to first supported format")
		return supportDetails.SurfaceFormats.front();
	}

	VkExtent2D PickBackbufferExtent(const SizeInt& preferredSize, const SwapchainSupportDetails& supportDetails) noexcept
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

	uint32_t PickBackbufferCount(uint8 preferredCount, const SwapchainSupportDetails& supportDetails) noexcept
	{
		uint32_t imageCount = Math::Max(static_cast<uint32_t>(preferredCount), supportDetails.SurfaceCapabilities.minImageCount);

		if (supportDetails.SurfaceCapabilities.maxImageCount > 0)
			imageCount = Math::Min(imageCount, supportDetails.SurfaceCapabilities.maxImageCount);

		return imageCount;
	}

	SwapchainSupportDetails GetSwapchainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
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

	bool VulkanGraphicsPresenter::EnsureSwapchain()
	{
		if (_swapchain && !_isSwapchainDirty)
			return true;

		return RecreateSwapchain();
	}

	bool VulkanGraphicsPresenter::RecreateSwapchain()
	{
		if (!_surface)
		{
			CocoError("Window surface must be initialized before creating a swapchain")
			return false;
		}

		DeviceQueue* presentQueue = _device->GetOrCreatePresentQueue(_surface);

		if (!presentQueue)
		{
			throw std::exception("Device does not support presentation");
		}

		DeviceQueue* graphicsQueue = _device->GetQueue(DeviceQueue::Type::Graphics);

		if (!graphicsQueue)
		{
			throw std::exception("Device does not support graphics operations");
		}

		SwapchainSupportDetails swapchainSupport = GetSwapchainSupportDetails(_device->GetPhysicalDevice(), _surface);

		if (swapchainSupport.PresentModes.size() == 0 || swapchainSupport.SurfaceFormats.size() == 0)
		{
			throw std::exception("Device does not have adequate swapchain support");
		}

		// Wait until all graphics operations have finished
		_device->WaitForIdle();

		DestroySwapchainObjects();

		VkPresentModeKHR presentMode = PickPresentMode(_vSyncMode, swapchainSupport);
		VkSurfaceFormatKHR surfaceFormat = PickSurfaceFormat(swapchainSupport);
		VkExtent2D extent = PickBackbufferExtent(_framebufferSize, swapchainSupport);

		// TODO: configure backbuffer count based on desired vsync mode?
		uint32 imageCount = PickBackbufferCount(2, swapchainSupport);

		std::vector<uint32_t> queueFamilyIndices;

		VkSwapchainKHR oldSwapchain = _swapchain;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
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

		if (presentQueue->FamilyIndex != graphicsQueue->FamilyIndex)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;

			queueFamilyIndices.emplace_back(presentQueue->FamilyIndex);
			queueFamilyIndices.emplace_back(graphicsQueue->FamilyIndex);
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

			queueFamilyIndices.emplace_back(presentQueue->FamilyIndex);
		}

		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();

		VkResult result = vkCreateSwapchainKHR(_device->GetDevice(), &createInfo, _device->GetAllocationCallbacks(), &_swapchain);

		if (oldSwapchain)
			vkDestroySwapchainKHR(_device->GetDevice(), oldSwapchain, _device->GetAllocationCallbacks());

		if (result != VK_SUCCESS)
		{
			CocoError("Failed to create swapchain: {}", string_VkResult(result))
			_swapchain = nullptr;

			DestroyRenderContexts();

			return false;
		}

		_framebufferSize = SizeInt(extent.width, extent.height);
		_vSyncMode = ToVSyncMode(presentMode);

		_backbufferDescription = ImageDescription(
			_framebufferSize.Width, _framebufferSize.Height,
			1,
			GetPixelFormat(surfaceFormat.format),
			GetColorSpace(surfaceFormat.format),
			_sBackbufferUsageFlags
		);
		try
		{
			// Get backbuffer images
			uint32_t backbufferImageCount;
			AssertVkSuccess(vkGetSwapchainImagesKHR(_device->GetDevice(), _swapchain, &backbufferImageCount, nullptr));

			std::vector<VkImage> images(backbufferImageCount);
			AssertVkSuccess(vkGetSwapchainImagesKHR(_device->GetDevice(), _swapchain, &backbufferImageCount, images.data()));

			for (size_t i = 0; i < images.size(); i++)
			{
				_backbuffers.emplace_back(CreateManagedRef<VulkanImage>(ID, _backbufferDescription, images[i]));
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Failed to obtain swapchain images: {}", ex.what())

			DestroySwapchainObjects();
			DestroyRenderContexts();

			return false;
		}

		if (_renderContexts.size() != _backbuffers.size())
		{
			DestroyRenderContexts();

			if (!CreateRenderContexts())
			{
				DestroySwapchainObjects();
				_isSwapchainDirty = true;
				return false;
			}
		}

		_isSwapchainDirty = false;

		CocoTrace("Created Vulkan swapchain with {} backbuffers at {}", _backbuffers.size(), _framebufferSize.ToString())

		return true;
	}

	void VulkanGraphicsPresenter::MarkDirty()
	{
		_isSwapchainDirty = true;
	}

	bool VulkanGraphicsPresenter::CreateRenderContexts()
	{
		try
		{
			for (size_t i = 0; i < _backbuffers.size(); i++)
			{
				_renderContexts.emplace_back(CreateManagedRef<VulkanRenderContext>(ID));
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Error creating VulkanRenderContext: {}", ex.what())

			return false;
		}

		_currentContextIndex = 0;
		CocoTrace("Created {} VulkanRenderContexts", _renderContexts.size())

		return true;
	}

	void VulkanGraphicsPresenter::DestroyRenderContexts()
	{
		if (_renderContexts.size() == 0)
			return;

		CocoTrace("Releasing {} VulkanRenderContexts", _renderContexts.size())
		
		_renderContexts.clear();
	}

	void VulkanGraphicsPresenter::DestroySwapchainObjects()
	{
		_backbuffers.clear();
		_acquiredBackbufferIndices.clear();

		_isSwapchainDirty = true;
	}
}