#include "Resource.h"

namespace Coco
{
	std::atomic<ResourceID> Resource::s_ResourceIndex = 0;

	Resource::Resource() noexcept
	{
		_id = GetNewID();
	}

	ResourceID Resource::GetNewID() noexcept
	{
		return s_ResourceIndex.fetch_add(1);
	}
}