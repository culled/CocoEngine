#pragma once

#ifdef _WIN64
#define COCO_PLATFORM_WINDOWS
#include <Coco/Platforms/Windows/CoreWindows.h>
#else
#error "Windows x64 is the only supported platform so far"
#endif

// Core types
#include <Coco/Core/Asserts.h>
#include <Coco/Core/Types/Exceptions.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Ref.h>