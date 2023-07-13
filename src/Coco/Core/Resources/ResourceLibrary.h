#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Types/Map.h>
#include "Resource.h"
#include "IResourceSerializer.h"
#include <atomic>
#include <Coco/Core/IO/File.h>
#include <Coco/Core/IO/FilePath.h>
#include <Coco/Core/MainLoop/MainLoopTickListener.h>

namespace Coco::Logging
{
	class Logger;
}

namespace Coco
{
	/// @brief A library that holds Resources
	class COCOAPI ResourceLibrary
	{
	public:
		/// @brief An infinite lifetime for resources
		constexpr static uint64_t MaxLifetime = Math::MaxValue<uint64_t>();

		/// @brief A default resource lifetime
		constexpr static uint64_t DefaultResourceLifetimeTicks = 500;

		constexpr static double DefaultPurgePeriod = 5.0;

		constexpr static uint64_t PurgeTickPriority = 10000;

		/// @brief The base path for all relative file paths regarding this library
		const string BasePath;

	private:
		uint64_t _resourceLifetimeTicks;
		UnorderedMap<ResourceID, ManagedRef<Resource>> _resources;
		UnorderedMap<std::type_index, ManagedRef<IResourceSerializer>> _serializers;
		Ref<MainLoopTickListener> _purgeTickListener;

	public:
		ResourceLibrary(const string& basePath = "", uint64_t resourceLifetimeTicks = DefaultResourceLifetimeTicks, double purgePeriod = DefaultPurgePeriod) noexcept;
		virtual ~ResourceLibrary();

		/// @brief Gets the logger for the resource library
		/// @return This library's logger
		Logging::Logger* GetLogger() noexcept;

		/// @brief Gets a full file path from a relative path
		/// @param relativePath The relative path within this resource library's base path
		/// @return A full file path
		string GetFullFilePath(const string& relativePath) const noexcept { return BasePath + relativePath; }

		/// @brief Creates a resource serializer for this library
		/// @tparam SerializerType The type of serializer
		/// @tparam ...Args The type of arguments for the serializer's constructor
		/// @param ...args Arguments to pass to the serializer
		template<typename SerializerType, typename ... Args>
		void CreateSerializer(Args&& ... args)
		{
			static_assert(std::is_base_of_v<IResourceSerializer, SerializerType>, "Only classes derived from IResourceSerializer can be resource serializers");

			ManagedRef<SerializerType> serializer = CreateManagedRef<SerializerType>(std::forward<Args>(args)...);
			const std::type_index serializerType = serializer->GetResourceType();

			if (_serializers.contains(serializerType))
			{
				LogWarning(GetLogger(), FormattedString("A serializer for \"{}\" resource types has already been created", serializerType.name()));
				return;
			}

			_serializers.try_emplace(serializerType, std::move(serializer));
		}

		/// @brief Creates a resource
		/// @tparam ResourceType The type of resource
		/// @tparam ...Args The type of arguments to pass to the resource's constructor
		/// @param name The name of the resource
		/// @param ...args The arguments to pass to the resource's constructor
		/// @return The created resource
		template<typename ResourceType, typename ... Args>
		Ref<ResourceType> CreateResource(const string& name, Args&& ... args)
		{
			static_assert(std::is_base_of_v<Resource, ResourceType>, "Class is not a Resource");

			ResourceID id = GetNextResourceID();

			auto result = _resources.try_emplace(id, CreateManagedRef<ResourceType>(id, name, std::forward<Args>(args)...));

			Ref<ResourceType> resource = static_cast<Ref<ResourceType>>(result.first->second);
			resource->UpdateTickUsed();

			return resource;
		}

		/// @brief Gets a resource with the given ID (if one exists)
		/// @param id The id of the resource
		/// @return The resource with the given ID, or an empty reference if no resource exists
		Ref<Resource> GetResource(const ResourceID& id);

		/// @brief Gets a resource with the given ID (if one exists)
		/// @param id The ID of the resource
		/// @return The resource with the given ID, or an empty reference if no resource exists
		template<typename ResourceType>
		Ref<ResourceType> GetResource(const ResourceID& id)
		{
			static_assert(std::is_base_of_v<Resource, ResourceType>, "Class is not a Resource");

			Ref<Resource> resource = GetResource(id);

			if (resource->GetType() != typeid(ResourceType))
				return Ref<ResourceType>();

			return static_cast<Ref<ResourceType>>(resource);
		}

		/// @brief Checks if a resource with the given ID exists
		/// @param id The ID of the resource 
		/// @return True if the resource exists
		bool HasResource(const ResourceID& id) const;

		/// @brief Gets the number of resources in this library
		/// @return The number of resources in this library
		uint64_t GetResourceCount() const noexcept { return _resources.size(); }

		/// @brief Purges the resource with the given ID. By default, resources are only destroyed if they have no outside users
		/// @param id The ID of the resource
		/// @param forcePurge If true, the resource will be destroyed regardless if the number of users using it. Be careful with this as it may invalidate references
		void PurgeResource(const ResourceID& id, bool forcePurge = false);

		/// @brief Purges any resources that haven't been used in a while and have no outside users
		/// @return The number of purged resources
		uint64_t PurgeStaleResources();

		/// @brief Purges all resources
		void PurgeResources();

		/// @brief Serializes a resource to a string. NOTE: a serializer for the resource type must have been created with this library for this to work
		/// @param resource The resource
		/// @return A string representation of the resource
		string SerializeResource(const Ref<Resource>& resource);

		/// @brief Deserializes a resource from a string. NOTE: a serializer for the resource type must have been created with this library for this to work
		/// @tparam ResourceType The type of resource to create
		/// @param name The name of resource
		/// @param data The serialized data
		/// @return The deserialized resource
		template<typename ResourceType>
		Ref<ResourceType> DeserializeResource(const string& data)
		{
			static_assert(std::is_base_of_v<Resource, ResourceType>, "Class is not a Resource");

			ManagedRef<Resource> rawResource = GetSerializerForResourceType(typeid(ResourceType))->Deserialize(*this, data);
			auto result = _resources.try_emplace(rawResource->ID, std::move(rawResource));

			Ref<ResourceType> resource = static_cast<Ref<ResourceType>>(result.first->second);
			resource->UpdateTickUsed();

			return resource;
		}

		/// @brief Loads a resource from a file
		/// @tparam ResourceType The type of resource to load
		/// @param path The path to the file relative to this library's base path
		/// @return The loaded resource
		template<typename ResourceType>
		Ref<ResourceType> Load(const string& path)
		{
			static_assert(std::is_base_of_v<Resource, ResourceType>, "Class is not a Resource");

			string fullPath = GetFullFilePath(path);

			auto it = std::find_if(_resources.begin(), _resources.end(), [fullPath](const auto& kvp) {
				return kvp.second->GetFilePath() == fullPath;
			});

			if (it != _resources.end())
			{
				Ref<Resource> resource = it->second;
				if (resource->GetType() != typeid(ResourceType))
				{
					LogError(GetLogger(), FormattedString(
						"Attempted to load file \"{}\" as {}, but it was already loaded as {}",
						path,
						typeid(ResourceType).name(),
						resource->GetType().name()));

					return Ref<ResourceType>();
				}

				return static_cast<Ref<ResourceType>>(resource);
			}

			string text = File::ReadAllText(fullPath);
			Ref<ResourceType> resource = DeserializeResource<ResourceType>(text);
			resource->SetFilePath(fullPath);

			return resource;
		}

		/// @brief Saves a resource to a file
		/// @param resource The resource to save
		/// @param filePath The path of the file relative to this library's base path
		void Save(const Ref<Resource>& resource, const string& filePath);

	private:
		/// @brief Tries to find a serializer for the given resource type
		/// @param resourceType The type of the resource
		/// @return A pointer to the serializer instance
		IResourceSerializer* GetSerializerForResourceType(std::type_index resourceType);

		/// @brief Gets the next resource ID to use
		/// @return The next resource ID
		ResourceID GetNextResourceID();

		/// @brief Handles the purge tick for stale resources
		/// @param deltaTime The time since the tick was last executed
		void PurgeTick(double deltaTime);
	};
}