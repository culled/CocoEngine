#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Engine.h>
#include "Resource.h"

namespace Coco
{
	/// @brief A fast cache for resources
	/// @tparam ResourceType The type of resource this cache will hold
	template<typename ResourceType>
	class ResourceCache
	{
		static_assert(std::is_base_of<Resource, ResourceType>::value, "Object must derive from Resource");

		struct Iterator
		{
		private:
			UnorderedMap<ResourceID, ResourceType>::iterator _iter;

		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = uint64_t;
			using value_type = ResourceType;
			using pointer = ResourceType*;
			using reference = ResourceType&;

			Iterator(UnorderedMap<ResourceID, ResourceType>::iterator iter) : _iter(iter) {}

			constexpr reference operator*() const { return (_iter->second); }
			constexpr pointer operator->() const { return &(_iter->second); }

			constexpr Iterator& operator++() { _iter++; return *this; }
			constexpr Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

			friend constexpr bool operator==(const Iterator& a, const Iterator& b) { return a._iter == b._iter; }
			friend constexpr bool operator!=(const Iterator& a, const Iterator& b) { return a._iter != b._iter; }

			friend constexpr bool operator<(const Iterator& a, const Iterator& b) { return a._iter < b._iter; }
			friend constexpr bool operator<=(const Iterator& a, const Iterator& b) { return a._iter <= b._iter; }

			friend constexpr bool operator>(const Iterator& a, const Iterator& b) { return a._iter > b._iter; }
			friend constexpr bool operator>=(const Iterator& a, const Iterator& b) { return a._iter >= b._iter; }
		};

	private:
		UnorderedMap<ResourceID, ResourceType> _resources;
		uint64_t _resourceLifetimeTicks;

	public:
		ResourceCache(uint64_t resourceLifetimeTicks) : _resourceLifetimeTicks(resourceLifetimeTicks)
		{}

		/// @brief Creates a resource
		/// @tparam ...Args 
		/// @param id The id of the resource to create
		/// @param ...args The arguments to pass to the resource's constructor
		/// @return The created resource
		template<typename ... Args>
		ResourceType* Create(ResourceID id, Args&& ... args)
		{
			if (_resources.contains(id))
				return Get(id);

			auto result = _resources.try_emplace(id, ResourceType(id, "", std::forward<Args>(args)...)).first;

			ResourceType* resource = &(result->second);
			resource->UpdateTickUsed();
			return resource;
		}

		/// @brief Gets a resource at a given ID. NOTE: make sure that the resource exists before using this function
		/// @param id The ID of the resource
		/// @return The resource
		ResourceType* Get(ResourceID id)
		{
			ResourceType* resource = &_resources.at(id);
			resource->UpdateTickUsed();
			return resource;
		}

		/// @brief Gets if a resource with the given ID exists
		/// @param id The ID of the resource
		/// @return True if the resource exists
		bool Has(ResourceID id) const
		{
			return _resources.contains(id);
		}

		/// @brief Destroys a resource with the given ID
		/// @param id The ID of the resource to destroy
		/// @return True if the resource was destroyed
		bool Destroy(ResourceID id)
		{
			if (!_resources.contains(id))
				return false;

			_resources.erase(id);
			return true;
		}

		/// @brief Immediately destroys all resources
		void Invalidate()
		{
			while (_resources.size() > 0)
			{
				_resources.erase(_resources.cbegin());
			}
		}

		/// @brief Purges any resources that haven't been used in a while and have no outside users
		/// @return The number of purged resources
		uint64_t PurgeStaleResources()
		{
			auto it = _resources.begin();
			uint64_t purgeCount = 0;

			const uint64_t currentTick = Engine::Get()->GetMainLoop()->GetTickCount();
			const uint64_t staleTickThreshold = currentTick - Math::Min(currentTick, _resourceLifetimeTicks);

			while (it != _resources.end())
			{
				// Only purge resources that haven't been used in a while and have no current users 
				if (it->second.GetLastTickUsed() < staleTickThreshold)
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

		Iterator begin() { return Iterator(_resources.begin()); }
		Iterator end() { return Iterator(_resources.end()); }
	};
}