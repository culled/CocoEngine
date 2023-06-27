#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Types/Map.h>
#include "Resource.h"
#include "FileResource.h"
#include "ResourceSerializer.h"
#include <atomic>
#include <Coco/Core/IO/File.h>

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
		static constexpr uint64_t DefaultTickLifetime = 10000;

		/// @brief The base path for all FileResources
		const string BasePath;

	private:
		std::atomic<ResourceID> _resourceID;
		UnorderedMap<ResourceID, ManagedRef<Resource>> _resources;
		UnorderedMap<std::type_index, ManagedRef<ResourceSerializer>> _serializers;

	public:
		ResourceLibrary(const string& basePath = "");
		virtual ~ResourceLibrary();

		/// @brief Gets the logger for the resource library
		/// @return This library's logger
		Logging::Logger* GetLogger() const noexcept;

		/// @brief Gets a full file path from a relative path
		/// @param relativePath The relative path within the resource library base path
		/// @return The full file path
		string GetFullFilePath(const string& relativePath) const noexcept { return BasePath + relativePath; }

		/// @brief Creates a resource serializer and adds it to this library's list of serializers
		/// @tparam SerializerType 
		/// @tparam ...Args 
		/// @param ...args Arguments to pass to the serializer
		template<typename SerializerType, typename ... Args>
		void AddSerializer(Args&& ... args)
		{
			static_assert(std::is_base_of_v<ResourceSerializer, SerializerType>, "Only classes derived from ResourceSerializer can be resource serializers");

			ManagedRef<SerializerType> serializer = CreateManagedRef<SerializerType>(this, std::forward<Args>(args)...);
			const std::type_index serializerType = serializer->GetResourceType();

			if (_serializers.contains(serializerType))
			{
				LogWarning(GetLogger(), FormattedString("A serializer for \"{}\" resource types has already been created", serializerType.name()));
				return;
			}

			_serializers.try_emplace(serializerType, std::move(serializer));
		}

		template<typename ResourceType, typename ... Args>
		Ref<ResourceType> CreateResource(const string& name, uint64_t tickLifetime, Args&& ... args)
		{
			ResourceID id = GetNextResourceID();

			auto result = _resources.try_emplace(id, CreateManagedRef<ResourceType>(id, name, tickLifetime, std::forward<Args>(args)...));

			if (id > _resourceID)
				_resourceID = id + 1;

			Ref<Resource> resource = result.first->second;
			resource->UpdateTickUsed();

			return static_cast<Ref<ResourceType>>(result.first->second);
		}

		template<typename ResourceType>
		Ref<ResourceType> DeserializeResource(const string& name, uint64_t tickLifetime, const string& data)
		{
			ResourceID id = GetNextResourceID();
			auto result = _resources.try_emplace(id, CreateManagedRef<ResourceType>(id, name, tickLifetime));

			Ref<ResourceType> resource = static_cast<Ref<ResourceType>>(result.first->second);
			DeserializeResource(data, resource);
			return resource;
		}

		template<typename ResourceType>
		Ref<ResourceType> GetResource(ResourceID id)
		{
			Ref<Resource> resource = GetResource(id);

			if (resource->GetType() != typeid(ResourceType))
				return Ref<ResourceType>();

			return static_cast<Ref<ResourceType>>(resource);
		}

		Ref<Resource> GetResource(ResourceID id);

		template<typename ResourceType>
		Ref<ResourceType> Load(uint64_t tickLifetime, const string& path)
		{
			string fullPath = GetFullFilePath(path);

			auto it = std::find_if(_resources.begin(), _resources.end(), [fullPath](const auto& kvp) {
				if (const FileResource* file = dynamic_cast<const FileResource*>(kvp.second.Get()))
				{
					return file->GetFilePath() == fullPath;
				}
				else
				{
					return false;
				}
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

			Ref<ResourceType> resource = CreateResource<ResourceType>("", tickLifetime);
			
			string text = File::ReadAllText(fullPath);
			DeserializeResource(text, resource.Get());

			if (FileResource* file = dynamic_cast<FileResource*>(resource.Get()))
			{
				file->SetFilePath(path);
			}

			return resource;
		}

		bool HasResource(ResourceID id) const;
		uint64_t GetResourceCount() const { return _resources.size(); }

		/// @brief Purges the resource. By default, resources are only destroyed if they have no outside users
		/// @param id The id of the resource
		/// @param forcePurge If true, the resource will be destroyed regardless if the number of users using it. Be careful with this as it may invalidate references
		void PurgeResource(ResourceID id, bool forcePurge = false);

		string SerializeResource(const Ref<Resource>& resource);
		void DeserializeResource(const string& data, Resource* resource);

		uint64_t PurgeStaleResources();

	private:
		/// @brief Tries to find a serializer for the given resource type
		/// @param resourceType The type of the resource
		/// @return A pointer to the serializer instance
		ResourceSerializer* GetSerializerForResourceType(std::type_index resourceType);

		ResourceID GetNextResourceID();
	};
}