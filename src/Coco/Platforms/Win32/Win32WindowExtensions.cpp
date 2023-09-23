#include "CPWinpch.h"
#include "Win32WindowExtensions.h"
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/RenderService.h>

#ifdef COCO_RENDERING_VULKAN
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatform.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPresenterSurface.h>
#include <vulkan/vulkan.h>
#include "WindowsIncludes.h"
#include <vulkan/vulkan_win32.h>
#endif

#include "Win32Window.h"
#include "Win32EnginePlatform.h"

namespace Coco::Platforms::Win32
{
	SharedRef<Rendering::GraphicsPresenterSurface> Win32WindowExtensions::CreateSurfaceForWindow(const char* renderRHIName, const Win32Window& window) const
	{
#ifdef COCO_RENDERING_VULKAN
		if (strcmp(renderRHIName, Rendering::Vulkan::VulkanGraphicsPlatform::sVulkanRHIName) == 0)
		{
			return CreateVulkanSurfaceForWindow(window);
		}
#endif

		string err = FormatString("Unsupported render RHI: {}", renderRHIName);
		throw std::exception(err.c_str());
	}

#ifdef COCO_RENDERING_VULKAN
	SharedRef<Rendering::GraphicsPresenterSurface> Win32WindowExtensions::CreateVulkanSurfaceForWindow(const Win32Window& window) const
	{
		using namespace Coco::Rendering;
		using namespace Coco::Rendering::Vulkan;

		if (!RenderService::Get())
			throw std::exception("No RenderService has been created");

		Win32EnginePlatform& platform = static_cast<Win32EnginePlatform&>(Engine::Get()->GetPlatform());

		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = window.GetHandle();
		createInfo.hinstance = platform.GetHInstance();

		SharedRef<VulkanGraphicsPresenterSurface> surface = CreateSharedRef<VulkanGraphicsPresenterSurface>();

		VulkanGraphicsPlatform& graphicsPlatform = static_cast<VulkanGraphicsPlatform&>(RenderService::Get()->GetPlatform());
		AssertVkSuccess(vkCreateWin32SurfaceKHR(graphicsPlatform.GetVulkanInstance(), &createInfo, graphicsPlatform.GetAllocationCallbacks(), &(surface->Surface)))

		return surface;
	}
#endif
}