#pragma once

#include "../Corepch.h"
#include "../Types/Refs.h"
#include "Resource.h"

namespace Coco
{
	/// @brief A library that holds Resources
	class ResourceLibrary
	{
		template<typename ResourceType>
		friend class ResourceIterator;

	public:
		ResourceLibrary();
		~ResourceLibrary();
		
		bool Add(SharedRef<Resource> resource);

		template<typename ResourceType, typename ... Args>
		SharedRef<ResourceType> Create(const ResourceID& id, Args&& ... args)
		{
			static_assert(std::is_base_of<Resource, ResourceType>::value, "Object type must derive from Resource");

			if (Has(id))
			{
				return nullptr;
			}

			SharedRef<ResourceType> resource = CreateSharedRef<ResourceType>(id, std::forward<Args>(args)...);
			Add(resource);

			return resource;
		}

		bool Has(const ResourceID& id) const;

		SharedRef<Resource> Get(const ResourceID& id) const;

		bool TryGet(const ResourceID& id, SharedRef<Resource>& outResource) const;

		template<typename ResourceType>
		SharedRef<ResourceType> GetAs(const ResourceID& id) const
		{
			SharedRef<Resource> r = Get(id);

			if (!r)
				return nullptr;


			const std::type_info& type = r->GetType();
			if (type != typeid(ResourceType))
			{
				return nullptr;
			}

			return std::static_pointer_cast<ResourceType>(r);
		}

		template<typename ResourceType>
		bool TryGetAs(const ResourceID& id, SharedRef<ResourceType>& outResource) const
		{
			if (!Has(id))
				return false;

			outResource = GetAs<ResourceType>(id);
			return true;
		}

		uint64 Count() const;

		bool Remove(const ResourceID& id);

		uint64 RemoveUnusedResources();

	private:
		std::unordered_map<ResourceID, SharedRef<Resource>> _resources;
	};

	template<typename ResourceType>
	class ResourceIterator
	{
	public:
		using ResourceMap = std::unordered_map<ResourceID, SharedRef<Resource>>;
		using difference_type = std::ptrdiff_t;
		using value_type = SharedRef<ResourceType>;
		using pointer = value_type*;
		using reference = value_type;

	public:
		ResourceIterator(const ResourceLibrary& library) :
			_map(library._resources),
			_it(library._resources.begin())
		{
			FindValidResource(true);
		}

		bool operator==(const ResourceIterator& other) const { return _it == other._it; }
		bool operator!=(const ResourceIterator& other) const { return !(*this == other); }

		reference operator*() const { return std::static_pointer_cast<ResourceType>(_it->second); }
		pointer operator->() const { return &operator*(); }

		ResourceIterator& operator++() { FindValidResource(false); return *this; }
		ResourceIterator& operator++(int) { ResourceIterator tmp = *this; ++(*this); return tmp; }

		bool AtEnd() const { return _it == _map.end(); }

	private:
		ResourceMap::const_iterator _it;
		const ResourceMap& _map;

	private:
		ResourceIterator(const ResourceIterator& other) :
			_map(other._map),
			_it(other._it)
		{}

		void FindValidResource(bool includeCurrent)
		{
			bool first = true;

			while (!AtEnd())
			{
				if (!first || !includeCurrent)
					++_it;

				first = false;

				if (_it->second->GetType() == typeid(ResourceType))
					break;
			}
		}
	};
}