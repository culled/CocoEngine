#pragma once

#include "Core.h"

namespace Coco
{
	/// <summary>
	/// Exit codes for the engine/application
	/// </summary>
	enum class ExitCode
	{
		Ok = 0,
		FatalError = 1,
		OtherError = 999
	};
}