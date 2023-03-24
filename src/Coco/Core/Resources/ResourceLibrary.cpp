#include "ResourceLibrary.h"

#include <Coco/Core/Engine.h>

namespace Coco
{
	ResourceLibrary::ResourceLibrary(const string& basePath) : BasePath(basePath)
	{
		// TODO: Load default loaders
	}

	Logging::Logger* ResourceLibrary::GetLogger() const noexcept
	{
		return Engine::Get()->GetLogger();
	}

	Ref<Resource> ResourceLibrary::GetOrLoadResource(const string& resourceType, const string& path)
	{
		// Get the resource if it already is loaded
		if (_loadedResources.contains(path))
			return _loadedResources.at(path);

		ResourceLoader* loader = GetLoaderForResourceType(resourceType);

		Ref<Resource> resource = loader->Load(path);
		_loadedResources.emplace(path, resource);

		return resource;
	}

	bool ResourceLibrary::Get(ResourceID id, Ref<Resource>& resource) const
	{
		for (const auto& kvp : _loadedResources)
		{
			if (kvp.second->ID == id)
			{
				resource = kvp.second;
				return true;
			}
		}

		return false;
	}

	void ResourceLibrary::SaveResource(Ref<Resource> resource, const string& path)
	{
		if (path.empty())
			throw InvalidOperationException("Path must not be empty");

		ResourceLoader* loader = GetLoaderForResourceType(resource->TypeName);

		// Save to disk
		loader->Save(resource, path);

		AddResource(resource, path);
	}

	void ResourceLibrary::AddResource(Ref<Resource> resource, const string& path)
	{
		if (path.empty())
			throw InvalidOperationException("Path must not be empty");

		_loadedResources[path] = resource;
	}

	void ResourceLibrary::RemoveResource(const string& path) noexcept
	{
		auto it = _loadedResources.find(path);

		if (it != _loadedResources.end())
		{
			_loadedResources.erase(it);
		}
		else
		{
			LogWarning(GetLogger(), FormattedString("Could not find a resource with path \"{}\" to remove", path));
		}
	}

	void ResourceLibrary::RemoveResource(ResourceID id) noexcept
	{
		for (auto it = _loadedResources.begin(); it != _loadedResources.end(); it++)
		{
			if ((*it).second->ID == id)
			{
				_loadedResources.erase(it);
				return;
			}
		}

		LogWarning(GetLogger(), FormattedString("Could not find a resource with id {} to remove", id));
	}

	ResourceLoader* ResourceLibrary::GetLoaderForResourceType(const string& resourceTypename)
	{
		const auto it = _resourceLoaders.find(resourceTypename);

		if (it == _resourceLoaders.end())
			throw InvalidOperationException(FormattedString(
				"A loader has not been registered for the object type \"{}\"",
				resourceTypename
			));

		return (*it).second.get();
	}
}
