#pragma once

#include <Coco/Core/Core.h>
#include <stdexcept>

namespace Coco
{
	using Exception = std::exception;
	using IndexOutOfRangeException = std::out_of_range;
	using PlatformInitializeException = Exception;
}