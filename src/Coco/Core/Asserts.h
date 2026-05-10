//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_ASSERTS_H
#define COCOENGINE_ASSERTS_H
#include "Types/CoreTypes.h"

namespace Coco
{
    /// @brief Logs an assertion failure
    /// @param expression The expression that failed the assertion
    /// @param file The source file that the expression is located in
    /// @param line The line number of the expression
    /// @param message The assertion message
    /// @param ... Arguments for formatting the assertion message
    void ReportAssertionFailure(const char* expression, const char* file, int32 line, const char* message, ...) noexcept;
}

#ifdef COCO_ASSERTIONS
#ifdef _MSC_VER
#define STATIC_ASSERT static_assert
#include <intrin.h>
#define DEBUG_BREAK __debugbreak();
#else
#define STATIC_ASSERT _Static_assert
#define DEBUG_BREAK __builtin_trap();
#endif

/// @brief Asserts that the Expression returns true, else breaks with a given message.
/// NOTE: do not use this for evaluation of values as it may get compiled out in release versions
#define COCO_ASSERT(Expression, Message, ...) 														\
do																								\
{																								\
if (Expression)	{}																			\
else																						\
{																							\
::Coco::ReportAssertionFailure(#Expression, __FILE__, __LINE__, Message, ##__VA_ARGS__);\
DEBUG_BREAK																				\
}																							\
} while (false)
#else
#define COCO_ASSERT(Expression, Message, ...)
#define STATIC_ASSERT
#define DEBUG_BREAK
#endif

#endif //COCOENGINE_ASSERTS_H