#include "Resource.h"
#include <Coco/Core/Engine.h>
#include "ResourceLoader.h"

namespace Coco
{
	std::atomic<ResourceID> Resource::s_ResourceIndex = 0;

	CachedResource::CachedResource(ResourceID id, ResourceVersion version) :
		ID(id), Version(version)
	{
		UpdateTickUsed();
	}

	void CachedResource::UpdateTickUsed()
	{
		LastTickUsed = Engine::Get()->GetMainLoop()->GetTickCount();
	}

	bool CachedResource::ShouldPurge(uint64_t staleTickThreshold) const noexcept
	{
		return IsInvalid() || Engine::Get()->GetMainLoop()->GetTickCount() - LastTickUsed > staleTickThreshold;
	}

	Resource::Resource(const string & namedType) noexcept : 
		_id(GetNewID()), _typename(namedType)
	{}
	
	Resource::Resource(ResourceType type) noexcept : Resource(ResourceTypeToString(type))
	{}

	ResourceID Resource::GetNewID() noexcept
	{
		return s_ResourceIndex.fetch_add(1);
	}
}