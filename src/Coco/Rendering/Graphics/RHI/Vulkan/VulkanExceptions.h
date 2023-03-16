#pragma once

#include <Coco/Core/Core.h>
#include <vulkan/vulkan.h>

namespace Coco::Rendering
{
	/// <summary>
	/// An exception when a Vulkan operation fails
	/// </summary>
	class VulkanOperationException : public Exception
	{
	public:
		VulkanOperationException(VkResult result, const char* messageFormat = "Vulkan operation failed: {}");
		virtual ~VulkanOperationException() = default;
	};
}