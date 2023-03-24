#include "Resource.h"
#include <Coco/Core/Engine.h>
#include "ResourceLoader.h"

namespace Coco
{
	std::atomic<ResourceID> Resource::s_resourceIndex = 0;
	
	Resource::Resource(const string& name, const string & namedType) noexcept : 
		ID(GetNewID()), TypeName(namedType), Name(name)
	{}
	
	Resource::Resource(const string& name, ResourceType type) noexcept : Resource(name, ResourceTypeToString(type))
	{}

	ResourceID Resource::GetNewID() noexcept
	{
		return s_resourceIndex.fetch_add(1);
	}

	CachedResource::CachedResource(ResourceID id, ResourceVersion version) :
		ID(id), _version(version)
	{
		UpdateTickUsed();
	}

	void CachedResource::UpdateTickUsed()
	{
		_lastTickUsed = Engine::Get()->GetMainLoop()->GetTickCount();
	}

	bool CachedResource::ShouldPurge(uint64_t staleTickThreshold) const noexcept
	{
		return IsInvalid() || Engine::Get()->GetMainLoop()->GetTickCount() - _lastTickUsed > staleTickThreshold;
	}
}