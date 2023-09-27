#include "Corepch.h"
#include "Resource.h"
#include "../Math/Math.h"

namespace Coco
{
	const ResourceID Resource::InvalidID = Math::MaxValue<ResourceID>();

	Resource::Resource(const ResourceID& id, const string& name) :
		_id(id),
		_name(name),
		_version(0),
		_contentPath()
	{}

	void Resource::SetName(const string& name)
	{
		_name = name;
	}

	void Resource::SetVersion(const ResourceVersion& version)
	{
		_version = version;
	}
}