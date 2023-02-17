#pragma once

#ifdef COCO_ASSERTIONS
#if _MSC_VER
#include <intrin.h>
// Causes an attached debugger to break
#define DebuggerBreak() __debugbreak()
#else
// Causes an attached debugger to break
#define DebuggerBreak() __builtin_trap()
#endif
#endif

#if COCO_ASSERTIONS
// Generic assertion without a log output
#define Assert(Expression)	\
{							\
	if(Expression)			\
	{ } else				\
	{						\
		DebuggerBreak();	\
	}						\
}
#else
// Generic assertion without a log output
#define Assert(Expression)
#endif