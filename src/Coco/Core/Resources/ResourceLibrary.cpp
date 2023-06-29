#include "ResourceLibrary.h"

#include <Coco/Core/Engine.h>

namespace Coco
{
	ResourceLibrary::ResourceLibrary(const string& basePath) : BasePath(basePath), _resourceID(0)
	{
		// TODO: Load default loaders
	}

	ResourceLibrary::~ResourceLibrary()
	{
		_resources.clear();
		_serializers.clear();
	}

	Logging::Logger* ResourceLibrary::GetLogger() const noexcept
	{
		return Engine::Get()->GetLogger();
	}

	Ref<Resource> ResourceLibrary::GetResource(ResourceID id)
	{
		if (!_resources.contains(id))
			return Ref<Resource>();

		Ref<Resource> resource = _resources.at(id);
		resource->UpdateTickUsed();
		return resource;
	}

	bool ResourceLibrary::HasResource(ResourceID id) const
	{
		return _resources.contains(id);
	}

	void ResourceLibrary::PurgeResource(ResourceID id, bool forcePurge)
	{
		if (!_resources.contains(id))
			return;

		Ref<Resource>& resource = _resources.at(id);

		if(forcePurge || resource.GetUseCount() <= 1)
			_resources.erase(id);
	}

	string ResourceLibrary::SerializeResource(const Ref<Resource>& resource)
	{
		return GetSerializerForResourceType(resource->GetType())->Serialize(resource);
	}

	void ResourceLibrary::DeserializeResource(const string& data, Resource* resource)
	{
		GetSerializerForResourceType(resource->GetType())->Deserialize(data, resource);
	}

	uint64_t ResourceLibrary::PurgeStaleResources()
	{
		auto it = _resources.begin();
		uint64_t purgeCount = 0;

		while (it != _resources.end())
		{
			// Only purge resources that haven't been used in a while and have no current users 
			if (it->second->IsStale() && it->second.GetUseCount() == 1)
			{
				it = _resources.erase(it);
				purgeCount++;
			}
			else
			{
				it++;
			}
		}

		return purgeCount;
	}

	ResourceSerializer* ResourceLibrary::GetSerializerForResourceType(std::type_index resourceType)
	{
		const auto it = _serializers.find(resourceType);

		if (it == _serializers.end())
			throw InvalidOperationException(FormattedString(
				"A serializer has not been registered for the object type \"{}\"",
				resourceType.name()
			));

		return (*it).second.Get();
	}

	ResourceID ResourceLibrary::GetNextResourceID()
	{
		return _resourceID.fetch_add(1);
	}
}
