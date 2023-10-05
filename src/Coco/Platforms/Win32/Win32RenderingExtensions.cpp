#include "CPWinpch.h"
#include "Win32RenderingExtensions.h"

#ifdef COCO_RENDERING_VULKAN
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatform.h>
#include <vulkan/vulkan.h>
#include "WindowsIncludes.h"
#include <vulkan/vulkan_win32.h>
#endif

namespace Coco::Platforms::Win32
{
	void Win32RenderingExtensions::GetRenderingExtensions(const char* renderRHIName, bool includePresentationExtensions, std::vector<const char*>& outExtensions) const
	{
#ifdef COCO_RENDERING_VULKAN
		if (strcmp(renderRHIName, Rendering::Vulkan::VulkanGraphicsPlatform::sVulkanRHIName) == 0)
		{
			AddVulkanRenderingExtensions(includePresentationExtensions, outExtensions);
		}
#endif
	}

#ifdef COCO_RENDERING_VULKAN
	void Win32RenderingExtensions::AddVulkanRenderingExtensions(bool includePresentationExtensions, std::vector<const char*>& outExtensions) const
	{
#ifdef COCO_SERVICE_WINDOWING
		if(includePresentationExtensions)
			outExtensions.push_back("VK_KHR_win32_surface");
#endif
	}
#endif
}