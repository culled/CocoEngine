//
// Created by cullen on 3/22/26.
//

#include "X11VulkanGraphicsSurfaceFactory.h"
#include <Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h>
#include <Coco/Rendering/RHI/Vulkan/VulkanRenderingExtensions.h>
#include "Coco/Rendering/RHI/Vulkan/VulkanExceptions.h"
#include "../X11Window.h"
#include "../X11Includes.h"

// HACK: Since we encapsulated X11 in a namespace, we need to make equivalent types for Vulkan to use
using Display = X11::Display;
using Window = X11::Window;
using VisualID = X11::VisualID;
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace Coco
{
    Ref<GraphicsSurface> X11VulkanGraphicsSurfaceFactory::Create(VulkanGraphicsPlatform& graphicsPlatform,
        const X11Window& window)
    {
        VkXlibSurfaceCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};
        createInfo.dpy = window.GetDisplay();
        createInfo.window = window.GetNativeWindow();
        VkSurfaceKHR surface;
        VkResult result = vkCreateXlibSurfaceKHR(graphicsPlatform.GetInstance(), &createInfo, graphicsPlatform.GetAllocationCallbacks(), &surface);

        if (result != VK_SUCCESS)
            throw VulkanOperationException(result);

        return graphicsPlatform.CreateSurface(surface, window.GetSize());
    }

    void X11VulkanGraphicsSurfaceFactory::GetExtensions(VulkanRenderingExtensions& extensions)
    {
        extensions.InstanceExtensions.Append(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
    }
} // Coco