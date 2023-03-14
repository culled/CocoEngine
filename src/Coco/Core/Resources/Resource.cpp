#include "Resource.h"

namespace Coco
{
	std::atomic<ResourceID> Resource::s_ResourceIndex = 0;

	Resource::Resource()
	{
		_id = GetNewID();
	}

	ResourceID Resource::GetNewID()
	{
		return s_ResourceIndex.fetch_add(1);
	}
}