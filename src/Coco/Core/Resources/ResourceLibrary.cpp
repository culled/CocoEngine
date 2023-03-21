#include "ResourceLibrary.h"

#include <Coco/Core/Engine.h>

namespace Coco
{
	ResourceLibrary::ResourceLibrary(const string& basePath) : BasePath(basePath)
	{
		// Load default loaders
	}

	Logging::Logger* ResourceLibrary::GetLogger() const noexcept
	{
		return Engine::Get()->GetLogger();
	}

	Ref<Resource> ResourceLibrary::GetOrLoadResource(ResourceType resourceType, const string& path)
	{
		return GetOrLoadResource(ResourceTypeToString(resourceType), path);
	}

	Ref<Resource> ResourceLibrary::GetOrLoadResource(const char* resourceType, const string& path)
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
			if (kvp.second->GetID() == id)
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

		ResourceLoader* loader = GetLoaderForResourceType(resource->GetTypename());

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

	void ResourceLibrary::RemoveResource(Ref<Resource> resource) noexcept
	{
		const string& resourcePath = resource->GetFilePath();

		if (resourcePath.empty())
		{
			LogWarning(GetLogger(), "Cannot remove a resource with no path");
			return;
		}

		RemoveResource(resourcePath);
	}

	void ResourceLibrary::RemoveResource(const string& path) noexcept
	{
		auto it = _loadedResources.find(path);

		if (it != _loadedResources.end())
			_loadedResources.erase(it);
	}

	ResourceLoader* ResourceLibrary::GetLoaderForResourceType(const char* resourceTypename)
	{
		auto it = _resourceLoaders.find(resourceTypename);

		if (it == _resourceLoaders.end())
			throw InvalidOperationException(FormattedString(
				"A loader has not been registered for the object type \"{}\"",
				resourceTypename
			));

		return (*it).second.get();
	}
}
