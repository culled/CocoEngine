#include "Corepch.h"
#include "Resource.h"
#include "../Math/Math.h"

namespace Coco
{
	Resource::Resource(const ResourceID& id) :
		_id(id),
		_version(0)
	{}

	void Resource::SetVersion(const ResourceVersion& version)
	{
		_version = version;
	}

	void Resource::IncrementVersion()
	{
		_version++;
	}
}