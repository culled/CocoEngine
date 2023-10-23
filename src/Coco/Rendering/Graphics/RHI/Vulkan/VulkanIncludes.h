#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <Coco/Core/Types/String.h>

// Throws an exception if the expression does not evaluate to a VK_SUCCESS
#define AssertVkSuccess(Expression) {	\
	VkResult result = Expression;		\
	if(result != VK_SUCCESS) {			\
		string err = FormatString("Vulkan operation error: {}", string_VkResult(result)); \
		throw std::exception(err.c_str());	\
	}	\
}