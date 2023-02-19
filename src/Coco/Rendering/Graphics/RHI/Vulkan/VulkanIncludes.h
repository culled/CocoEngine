#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#define CheckVKResult(Expression)																		\
VkResult result = Expression;																			\
if(result != VK_SUCCESS) {																				\
	string error = FormattedString("Vulkan operation failed with code {}", string_VkResult(result));	\
	throw Exception(error.c_str());																		\
}