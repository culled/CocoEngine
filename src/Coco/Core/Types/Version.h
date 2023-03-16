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
		int Major = 0;
		int Minor = 0;
		int Patch = 0;

		Version() = default;
		Version(int major, int minor, int patch) noexcept;
		virtual ~Version() = default;

		/// <summary>
		/// Gets a string representation of this version
		/// </summary>
		/// <returns>The string representation of this version</returns>
		string ToString() const noexcept { return FormattedString("{}.{}.{}", Major, Minor, Patch); }
	};
}