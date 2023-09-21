#include "Corepch.h"
#include "Resource.h"

namespace Coco
{
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