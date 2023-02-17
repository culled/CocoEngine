#pragma once

namespace Coco
{
	/// <summary>
	/// Exit codes for the engine/application
	/// </summary>
	enum class ExitCode
	{
		Ok = 0,
		FatalError = 1,
		UnhandledClientError = 2,
		OtherError = 999
	};
}