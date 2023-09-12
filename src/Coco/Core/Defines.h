#pragma once

// Unsigned base types

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

// Signed base types

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;

#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define COCO_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "Windows is 64-bit only!"
#endif
#else
// TODO: more platforms
#endif