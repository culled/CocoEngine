#pragma once

#ifdef _WIN64
#define COCO_PLATFORM_WINDOWS
#include <Coco/Platforms/Windows/APIWindows.h>
#else
#error "Windows x64 is the only supported platform so far"
#endif

using uint = unsigned int;
using ulong = unsigned long;

#include <stdint.h>