#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

// Windows-specific includes (mainly for surface creation)
#ifdef COCO_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Conflicts with std library
#undef min
#undef max

#include <vulkan/vulkan_win32.h>
#endif

// Ensures an expression that returns a VkResult returns VK_SUCCESS
#define CheckVKResult(Expression) {																		\
VkResult result = Expression;																			\
if(result != VK_SUCCESS) {																				\
	string error = FormattedString("Vulkan operation failed with code {}", string_VkResult(result));	\
	throw Exception(error.c_str());																		\
}																										\
}