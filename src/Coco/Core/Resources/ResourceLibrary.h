#pragma once

#include "../Corepch.h"
#include "../Types/Refs.h"
#include "Resource.h"
#include "ResourceSerializer.h"

namespace Coco
{
	/// @brief An ID generator for a TypedResourceLibrary
	/// @tparam IDType The type of ID
	template<typename IDType>
	struct ResourceLibraryIDGenerator
	{
		virtual ~ResourceLibraryIDGenerator() = default;

		virtual IDType operator()() = 0;
	};

	/// @brief A basic library for managing resources derived from a given type
	/// @tparam IDType The ID type
	/// @tparam BaseResourceType The base resource type
	/// @tparam IDGeneratorType The type of ID generator to use
	template<typename IDType, typename BaseResourceType, typename IDGeneratorType = ResourceLibraryIDGenerator<IDType>>
	class TypedResourceLibrary
	{
	protected:
		using ResourceMap = std::unordered_map<IDType, ManagedRef<BaseResourceType>>;

		IDGeneratorType _idGenerator;
		ResourceMap _resources;

	public:
		TypedResourceLibrary() :
			_idGenerator{},
			_resources{}
		{}

		virtual ~TypedResourceLibrary()
		{
			Clear();
		}

		/// @brief Creates a resource with the given arguments
		/// @tparam ResourceType The type of resource
		/// @tparam ...Args The types of arguments
		/// @param ...args The arguments to pass to the resource's constructor
		/// @return The created resource
		template<typename ResourceType, typename ... Args>
		Ref<ResourceType> Create(Args&&... args)
		{
			IDType id = _idGenerator();

			auto result = _resources.try_emplace(id, CreateManagedRef<ResourceType>(id, std::forward<Args>(args)...));
			ManagedRef<BaseResourceType>& rawResource = result.first->second;
			return Ref<ResourceType>(rawResource);
		}

		/// @brief Checks if a resource with the given ID exists in this library
		/// @param id The resource ID
		/// @return True if the resource exists in this library
		bool Has(const IDType& id) const { return _resources.contains(id); }

		/// @brief Gets a resource with the given ID.
		/// NOTE: Check if the resource exists first by calling Has()
		/// @param id The ID of the resource
		/// @return The resource
		virtual Ref<BaseResourceType> Get(const IDType& id)
		{
			return _resources.at(id);
		}

		/// @brief Gets a resource with the given ID.
		/// NOTE: Check if the resource exists first by calling Has()
		/// @tparam ResourceType The type of resource to cast to
		/// @param id The ID of the resource
		/// @return The resource
		template<typename ResourceType>
		Ref<ResourceType> GetAs(const IDType& id)
		{
			return static_cast<Ref<ResourceType>>(Get(id));
		}

		/// @brief Forcibly removes a resource from this library
		/// @param id The ID of the resource to remove
		virtual void Remove(const IDType& id)
		{
			auto it = _resources.find(id);

			if (it == _resources.end())
				return;

			_resources.erase(it);
		}

		/// @brief Clears all resources from this library
		virtual void Clear()
		{
			_resources.clear();
		}

		/// @brief Gets the number of resources in this library
		/// @return The number of resources in this library
		uint64 GetCount() const
		{
			return _resources.size();
		}

		/// @brief Purges all resources with no outside references
		/// @return The number of purged resources
		virtual uint64 PurgeUnused()
		{
			uint64 purgeCount = 0;

			auto it = _resources.begin();
			while (it != _resources.end())
			{
				if (it->second.GetUseCount() == 1)
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
	};

	/// @brief A generator for ResourceIDs
	struct ResourceIDGenerator
	{
		std::atomic<ResourceID> Counter;

		ResourceIDGenerator();

		ResourceID operator()();
	};

	/// @brief A library that holds Resources
	class ResourceLibrary : public TypedResourceLibrary<ResourceID, Resource, ResourceIDGenerator>
	{
	private:
		std::vector<UniqueRef<ResourceSerializer>> _serializers;

	public:
		~ResourceLibrary();

		/// @brief Creates a ResourceSerializer for this library
		/// @tparam SerializerType The type of serializer to create
		/// @tparam ...Args The types of arguments to pass to the serializer's constructor
		/// @param ...args The arguments to pass to the serializer's constructor
		template<typename SerializerType, typename ... Args>
		void CreateSerializer(Args&& ... args)
		{
			_serializers.emplace_back(CreateUniqueRef<SerializerType>(std::forward<Args>(args)...));
		}

		/// @brief Gets/loads a resource at the given content path
		/// @param contentPath The path of the resource
		/// @param outResource Will be set to the loaded resource if successful
		/// @return True if the resource was loaded/retrieved
		bool GetOrLoad(const string& contentPath, Ref<Resource>& outResource);

		/// @brief Saves a resource to a file
		/// @param contentPath The path of the file to save
		/// @param resource The resource to save
		/// @param overwrite If true, any file at the content path will be overwritten. If false, saving will fail if a file already exists
		/// @return True if the resource was saved
		bool Save(const string& contentPath, Ref<Resource> resource, bool overwrite);

	private:
		/// @brief Gets a ResourceSerializer that supports the given resource type
		/// @param type The resource type
		/// @return A resource serializer, or nullptr if no serializer supports the resource type
		ResourceSerializer* GetSerializerForResourceType(const std::type_index& type);

		/// @brief Gets a ResourceSerializer that supports the given file type
		/// @param contentPath The path to the file
		/// @return A resource serializer, or nullptr if no serializer supports the file type
		ResourceSerializer* GetSerializerForFileType(const string& contentPath);

		/// @brief Finds a resource with the given content path
		/// @param contentPath The content path
		/// @return An iterator to the resource if found, or an iterator at the end of the resource map if not found
		ResourceMap::iterator FindResource(const string& contentPath);
	};
}