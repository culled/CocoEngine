#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Map.h>
#include "Resource.h"
#include "ResourceLoader.h"

namespace Coco::Logging
{
	class Logger;
}

namespace Coco
{
	/// <summary>
	/// A library that can save and load resources to disk
	/// </summary>
	class COCOAPI ResourceLibrary
	{
	public:
		/// <summary>
		/// The base path for all resources
		/// </summary>
		const string BasePath;

	private:
		Map<string, Ref<Resource>> _loadedResources;
		Map<string, Managed<ResourceLoader>> _resourceLoaders;

	public:
		ResourceLibrary(const string& basePath);
		virtual ~ResourceLibrary() = default;

		/// <summary>
		/// Gets the logger for the resource library
		/// </summary>
		/// <returns>This library's logger</returns>
		Logging::Logger* GetLogger() const noexcept;

		/// <summary>
		/// Gets a full file path from a relative path
		/// </summary>
		/// <param name="relativePath">The relative path within the resource library base path</param>
		/// <returns>The full file path</returns>
		string GetFullFilePath(const string& relativePath) const noexcept { return BasePath + relativePath; }

		/// <summary>
		/// Creates a loader
		/// </summary>
		/// <param name="...args">Arguments to pass to the loader</param>
		template<typename LoaderType, typename ... Args>
		void CreateLoader(Args&& ... args)
		{
			static_assert(std::is_base_of_v<ResourceLoader, LoaderType>, "Only classes derived from ResourceLoader can be resource loaders");

			Managed<LoaderType> loader = CreateManaged<LoaderType>(this, BasePath, std::forward<Args>(args)...);
			const char* loaderType = loader->GetResourceTypename();

			if (_resourceLoaders.contains(loaderType))
			{
				LogWarning(GetLogger(), FormattedString("A loader for \"{}\" resource types has already been created", loaderType));
				return;
			}

			_resourceLoaders.emplace(loaderType, std::move(loader));
		}

		/// <summary>
		/// Returns a resource by either loading it from disk or retrieving it if is already loaded
		/// </summary>
		/// <param name="resourceType">The type of resource</param>
		/// <param name="path">The path to the resource, relative to this library's base path</param>
		/// <returns>The loaded resource</returns>
		Ref<Resource> GetOrLoadResource(ResourceType resourceType, const string& path);

		/// <summary>
		/// Returns a resource by either loading it from disk or retrieving it if is already loaded
		/// </summary>
		/// <param name="resourceType">The type of resource</param>
		/// <param name="path">The path to the resource, relative to this library's base path</param>
		/// <returns>The loaded resource</returns>
		Ref<Resource> GetOrLoadResource(const string& resourceType, const string& path);

		/// <summary>
		/// Gets a loaded resource by its ID
		/// </summary>
		/// <param name="id">The resource ID</param>
		/// <param name="resource">Will be set to the resource reference, if it is loaded</param>
		/// <returns>True if the resource was found</returns>
		bool Get(ResourceID id, Ref<Resource>& resource) const;

		/// <summary>
		/// Saves a resource to the given path and adds it to this library
		/// </summary>
		/// <param name="resource">The resource</param>
		/// <param name="path">The path of the saved file, relative to this library's base path</param>
		void SaveResource(Ref<Resource> resource, const string& path);

		/// <summary>
		/// Adds a resource to this library without saving it
		/// </summary>
		/// <param name="resource">The resource</param>
		/// <param name="path">The path of the saved file, relative to this library's base path</param>
		void AddResource(Ref<Resource> resource, const string& path);

		/// <summary>
		/// Removes a resource from this library
		/// </summary>
		/// <param name="resource">The resource to remove (must have a valid file path)</param>
		void RemoveResource(Ref<Resource> resource) noexcept;

		/// <summary>
		/// Removes a resource from this library
		/// </summary>
		/// <param name="path">The path of the resource to remove</param>
		void RemoveResource(const string& path) noexcept;

	private:
		/// <summary>
		/// Tries to find a loader for the given resource type
		/// </summary>
		/// <param name="resourceTypename">The type of the resource</param>
		/// <returns>A pointer to the loader instance</returns>
		ResourceLoader* GetLoaderForResourceType(const string& resourceTypename);
	};
}