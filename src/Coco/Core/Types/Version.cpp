#include "Version.h"

namespace Coco
{
	Version::Version(int major, int minor, int patch) noexcept :
		Major(major), Minor(minor), Patch(patch)
	{}

	string Version::ToString() const noexcept
	{
		return FormattedString("{}.{}.{}", Major, Minor, Patch);
	}
}