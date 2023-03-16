#include "VulkanExceptions.h"

#include <vulkan/vk_enum_string_helper.h>

namespace Coco::Rendering
{
	VulkanOperationException::VulkanOperationException(VkResult result, const char* messageFormat) :
		Exception(FormattedString(messageFormat, string_VkResult(result)))
	{}
}