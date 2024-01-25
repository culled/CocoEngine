#include "CPWinpch.h"
#include "Win32RenderingExtensions.h"

#ifdef COCO_RENDERING_VULKAN
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatform.h>
#endif

namespace Coco::Platforms::Win32
{
	void Win32RenderingExtensions::GetRenderingExtensions(const string& renderRHIName, bool includePresentationExtensions, std::vector<string>& outExtensions) const
	{
#ifdef COCO_RENDERING_VULKAN
		if (renderRHIName == Rendering::Vulkan::VulkanGraphicsPlatform::Name)
		{
			AddVulkanRenderingExtensions(includePresentationExtensions, outExtensions);
		}
#endif
	}

#ifdef COCO_RENDERING_VULKAN
	void Win32RenderingExtensions::AddVulkanRenderingExtensions(bool includePresentationExtensions, std::vector<string>& outExtensions) const
	{
#ifdef COCO_SERVICE_WINDOWING
		if(includePresentationExtensions)
			outExtensions.push_back("VK_KHR_win32_surface");
#endif
	}
#endif
}