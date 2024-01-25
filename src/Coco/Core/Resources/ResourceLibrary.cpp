#include "Corepch.h"
#include "ResourceLibrary.h"
#include "ResourceSerializer.h"
#include "../Engine.h"

namespace Coco
{
	ResourceLibrary::ResourceLibrary() :
		_resources()
	{}

	ResourceLibrary::~ResourceLibrary()
	{
		_resources.clear();
	}

	bool ResourceLibrary::Add(SharedRef<Resource> resource)
	{
		auto result = _resources.try_emplace(resource->_id, resource);
		return result.second;
	}

	bool ResourceLibrary::Has(const ResourceID& id) const
	{
		return _resources.contains(id);
	}

	SharedRef<Resource> ResourceLibrary::Get(const ResourceID& id) const
	{
		if (!Has(id))
		{
			CocoError("Resource \"{}\" does not exist", id.ToString())
			return nullptr;
		}

		return _resources.at(id);
	}

	bool ResourceLibrary::TryGet(const ResourceID& id, SharedRef<Resource>& outResource) const
	{
		if (!Has(id))
			return false;

		outResource = Get(id);
		return true;
	}

	uint64 ResourceLibrary::Count() const
	{
		return _resources.size();
	}

	bool ResourceLibrary::Remove(const ResourceID& id)
	{
		return _resources.erase(id) > 0;
	}

	uint64 ResourceLibrary::RemoveUnusedResources()
	{
		std::vector<SharedRef<Resource>> removeResources;

		auto it = _resources.begin();

		while (it != _resources.end())
		{
			if (it->second.use_count() <= 1)
			{
				// Defer the actual destruction until after we've found all orphaned resources
				removeResources.push_back(it->second);
				it = _resources.erase(it);
			}
			else
			{
				++it;
			}
		}

		uint64 removedResources = removeResources.size();

		// Actually destroy all orphaned resources
		removeResources.clear();

		return removedResources;
	}
}