#include "ResourceLibrary.h"

#include <Coco/Core/Engine.h>

namespace Coco
{
	ResourceLibrary::ResourceLibrary(const string& basePath, uint64_t resourceLifetimeTicks) noexcept :
		BasePath(basePath), 
		_resourceID(0), 
		_resourceLifetimeTicks(resourceLifetimeTicks)
	{
		// TODO: Load default serializers
	}

	ResourceLibrary::~ResourceLibrary()
	{
		_resources.clear();
		_serializers.clear();
	}

	Logging::Logger* ResourceLibrary::GetLogger() noexcept
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

		if(forcePurge || _resources.at(id).GetUseCount() <= 1)
			_resources.erase(id);
	}
	
	uint64_t ResourceLibrary::PurgeStaleResources()
	{
		auto it = _resources.begin();
		uint64_t purgeCount = 0;

		const uint64_t currentTick = Engine::Get()->GetMainLoop()->GetTickCount();
		const uint64_t staleTickThreshold = currentTick - Math::Min(currentTick, _resourceLifetimeTicks);

		while (it != _resources.end())
		{
			// Only purge resources that haven't been used in a while and have no current users 
			if (it->second->GetLastTickUsed() < staleTickThreshold && it->second.GetUseCount() == 1)
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

	void ResourceLibrary::PurgeResources()
	{
		// Purge individually in-case resources purge each other
		while (_resources.size() > 0)
		{
			_resources.erase(_resources.cbegin());
		}
	}

	string ResourceLibrary::SerializeResource(const Ref<Resource>& resource)
	{
		return GetSerializerForResourceType(resource->GetType())->Serialize(this, resource);
	}

	void ResourceLibrary::DeserializeResource(const string& data, Ref<Resource> resource)
	{
		GetSerializerForResourceType(resource->GetType())->Deserialize(this, data, resource);
	}

	void ResourceLibrary::Save(const Ref<Resource>& resource, const string& filePath)
	{
		const string fullPath = GetFullFilePath(filePath);

		const string data = SerializeResource(resource);

		File::WriteAllText(fullPath, data);
	}

	IResourceSerializer* ResourceLibrary::GetSerializerForResourceType(std::type_index resourceType)
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
