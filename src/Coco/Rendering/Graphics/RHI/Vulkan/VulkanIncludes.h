#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

// Windows-specific includes (mainly for surface creation)
#ifdef COCO_PLATFORM_WINDOWS

// Require at least Windows 7
#define NTDDI_WIN7 0x06010000
#define _WIN32_WINNT_WIN7 0x0601

// Trim down windows include
#define _AMD64_
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#define NODRAWTEXT
#define NOCTLMGR
#define NOFLATSBAPIS
#define NOSERVICE

#include <WinDef.h>
#include <WTypesbase.h>

#include <vulkan/vulkan_win32.h>
#endif

// Ensures an expression that returns a VkResult returns VK_SUCCESS
#define AssertVkResult(Expression) {														\
VkResult result = Expression;																\
if(result != VK_SUCCESS) {																	\
	string error = FormattedString("Vulkan operation failed: {}", string_VkResult(result));	\
	throw Exception(error.c_str());															\
}																							\
}