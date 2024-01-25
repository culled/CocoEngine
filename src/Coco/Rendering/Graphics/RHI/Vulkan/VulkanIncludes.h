#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "VulkanExceptions.h"

#include <Coco/Core/Types/String.h>

// Throws a VulkanOperationException if the expression does not evaluate to a VK_SUCCESS
#define AssertVkSuccess(Expression) {	\
	VkResult result = Expression;		\
	if(result != VK_SUCCESS) {			\
		throw ::Coco::Rendering::Vulkan::VulkanOperationException(result, ::Coco::FormatString("Vulkan operation error: {}", string_VkResult(result)));	\
	}	\
}