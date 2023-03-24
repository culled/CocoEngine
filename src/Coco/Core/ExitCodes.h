#pragma once

#include "Core.h"

namespace Coco
{
	/// @brief Exit codes for the engine/application
	enum class ExitCode
	{
		Ok = 0,
		FatalError = 1,
		OtherError = 999
	};
}