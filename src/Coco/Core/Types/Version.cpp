#include "Version.h"

namespace Coco
{
	Version::Version() : Version(0, 0, 0)
	{}

	Version::Version(int major, int minor, int patch) :
		Major(major), Minor(minor), Patch(patch)
	{}

	string Version::ToString() const
	{
		return FormattedString("{}.{}.{}", Major, Minor, Patch);
	}
}