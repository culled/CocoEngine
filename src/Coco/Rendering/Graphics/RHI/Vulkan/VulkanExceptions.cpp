#include "Renderpch.h"
#include "VulkanExceptions.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	VulkanException::VulkanException(const string& message) :
		Exception(message)
	{}

	VulkanOperationException::VulkanOperationException(VkResult result, const string& message) :
		VulkanException(message),
		_result(result)
	{}
}