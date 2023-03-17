#include "VulkanExceptions.h"

#include <vulkan/vk_enum_string_helper.h>

namespace Coco::Rendering::Vulkan
{
	VulkanRenderingException::VulkanRenderingException(const string& message) : RenderingException(message)
	{}

	VulkanOperationException::VulkanOperationException(VkResult result, const char* messageFormat) :
		VulkanRenderingException(FormattedString(messageFormat, string_VkResult(result)))
	{}

	DescriptorSetAllocateException::DescriptorSetAllocateException(const string& message) : VulkanRenderingException(message)
	{}
}