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
		_contentPath(),
		_savedVersion(0)
	{}

	void Resource::SetName(const string& name)
	{
		_name = name;
	}

	bool Resource::NeedsSaving() const
	{
		return _contentPath.IsEmpty() || _version != _savedVersion;
	}

	void Resource::SetVersion(const ResourceVersion& version)
	{
		_version = version;
	}

	void Resource::MarkSaved(const FilePath& contentPath)
	{
		_contentPath = contentPath;
		_savedVersion = _version;
	}
}