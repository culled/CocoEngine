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
	/// @brief A library that can save and load resources to disk
	class COCOAPI ResourceLibrary
	{
	public:
		/// @brief The base path for all resources
		const string BasePath;

	private:
		UnorderedMap<string, Ref<Resource>> _loadedResources;
		UnorderedMap<string, Managed<ResourceLoader>> _resourceLoaders;

	public:
		ResourceLibrary(const string& basePath);
		virtual ~ResourceLibrary() = default;

		/// @brief Gets the logger for the resource library
		/// @return This library's logger
		Logging::Logger* GetLogger() const noexcept;

		/// @brief Gets a full file path from a relative path
		/// @param relativePath The relative path within the resource library base path
		/// @return The full file path
		string GetFullFilePath(const string& relativePath) const noexcept { return BasePath + relativePath; }

		/// @brief Creates a resource loader and adds it to this library's list of loaders
		/// @tparam LoaderType 
		/// @tparam ...Args 
		/// @param ...args Arguments to pass to the loader
		template<typename LoaderType, typename ... Args>
		void CreateLoader(Args&& ... args)
		{
			static_assert(std::is_base_of_v<ResourceLoader, LoaderType>, "Only classes derived from ResourceLoader can be resource loaders");

			Managed<LoaderType> loader = CreateManaged<LoaderType>(this, std::forward<Args>(args)...);
			const string loaderType = loader->GetResourceTypename();

			if (_resourceLoaders.contains(loaderType))
			{
				LogWarning(GetLogger(), FormattedString("A loader for \"{}\" resource types has already been created", loaderType));
				return;
			}

			_resourceLoaders.emplace(loaderType, std::move(loader));
		}

		/// @brief Returns a resource by either retrieving it or loading it from disk
		/// @param resourceType The type of resource
		/// @param path The path to the resource, relative to this library's base path
		/// @return The loaded resource
		Ref<Resource> GetOrLoadResource(const ResourceType resourceType, const string& path)
		{
			return GetOrLoadResource(ResourceTypeToString(resourceType), path);
		}

		/// @brief Returns a resource by either retrieving it or loading it from disk
		/// @param resourceType The type of resource
		/// @param path The path to the resource, relative to this library's base path
		/// @return The loaded resource
		Ref<Resource> GetOrLoadResource(const string& resourceType, const string& path);

		/// @brief Gets a loaded resource by its ID
		/// @param id The resource ID
		/// @param resource Will be set to the resource reference, if it is loaded
		/// @return True if the resource was found
		bool Get(ResourceID id, Ref<Resource>& resource) const;

		/// @brief Saves a resource to the given path and adds it to this library
		/// @param resource The resource
		/// @param path The path of the saved file, relative to this library's base path
		void SaveResource(Ref<Resource> resource, const string& path);

		/// @brief Adds a resource to this library without saving it
		/// @param resource The resource
		/// @param path The path of resource
		void AddResource(Ref<Resource> resource, const string& path);

		/// @brief Removes a resource from this library
		/// @param resource The resource to remove
		void RemoveResource(Ref<Resource> resource) noexcept
		{
			RemoveResource(resource->ID);
		}

		/// @brief Removes a resource from this library via its file path
		/// @param path The path of the resource to remove
		void RemoveResource(const string& path) noexcept;

		/// @brief Removes a resource from this library via its ID
		/// @param id The id of the resource to remove
		void RemoveResource(ResourceID id) noexcept;

	private:
		/// @brief Tries to find a loader for the given resource type
		/// @param resourceTypename The type of the resource
		/// @return A pointer to the loader instance
		ResourceLoader* GetLoaderForResourceType(const string& resourceTypename);
	};
}