#include "Corepch.h"
#include "Version.h"

namespace Coco
{
	Version::Version() :
		Major(0),
		Minor(0),
		Patch(0)
	{}

	Version::Version(uint32 major, uint32 minor, uint32 patch) :
		Major(major),
		Minor(minor),
		Patch(patch)
	{}

	string Version::ToString() const
	{
		return FormatString("{}.{}.{}", Major, Minor, Patch);
	}
}