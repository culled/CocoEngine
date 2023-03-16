#pragma once

#include <vulkan/vulkan.h>
#include "VulkanExceptions.h"

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
const VkResult result = Expression;															\
if(result != VK_SUCCESS) {																	\
	throw VulkanOperationException(result);													\
}																							\
}