#pragma once

#include "../Corepch.h"
#include "../Types/Refs.h"

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
		IDGeneratorType _idGenerator;
		std::unordered_map<IDType, ManagedRef<BaseResourceType>> _resources;

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
		/// @tparam ResourceType The type of resource to cast to
		/// @param id The ID of the resource
		/// @return The resource
		template<typename ResourceType>
		Ref<ResourceType> Get(const IDType& id)
		{
			return Ref<ResourceType>(_resources.at(id));
		}

		void Remove(const IDType& id)
		{
			auto it = _resources.find(id);

			if (it == _resources.end())
				return;

			_resources.erase(it);
		}

		/// @brief Clears all resources from this library
		void Clear()
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
		uint64 PurgeUnused()
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
}