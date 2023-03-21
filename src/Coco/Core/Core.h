#pragma once

using uint = unsigned int;
using ulong = unsigned long;

#ifdef _WIN64
#define COCO_PLATFORM_WINDOWS
#include <Coco/Platforms/Windows/CoreWindows.h>
#else
#error "Windows x64 is the only supported platform so far"
#endif

#include <stdint.h>

// Core types
#include "Asserts.h"
#include "Types/Exceptions.h"
#include "Types/String.h"
#include "Types/Ref.h"
#include "Types/ManagedRef.h"
#include "Math/Math.h"
