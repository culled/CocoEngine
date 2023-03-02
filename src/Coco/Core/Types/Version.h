#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/String.h>

namespace Coco
{
	/// <summary>
	/// Represents a version with a major, minor, and patch number
	/// </summary>
	struct COCOAPI Version
	{
		int Major, Minor, Patch;

		Version();
		Version(int major, int minor, int patch);

		/// <summary>
		/// Gets a string representation of this version
		/// </summary>
		/// <returns>The string representation of this version</returns>
		string ToString() const;
	};
}