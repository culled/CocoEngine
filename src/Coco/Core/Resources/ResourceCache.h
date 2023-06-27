#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include "Resource.h"

namespace Coco
{
	template<typename ResourceType>
	class ResourceCache
	{
		static_assert(std::is_base_of<Resource, ResourceType>::value, "Object must derive from Resource");

	private:
		List<ResourceType> _resources;
		uint64_t _resourceLifetime;

	public:
		ResourceCache(uint64_t resourceLifetime) : _resourceLifetime(resourceLifetime)
		{}

		template<typename ... Args>
		ResourceType* Create(ResourceID id, Args&& ... args)
		{
			auto it = FindResource(id);

			if (it != _resources.end())
				return &(*it);

			_resources.Add(ResourceType(id, "", _resourceLifetime, std::forward<Args>(args)...));

			ResourceType* resource = &_resources.Last();
			resource->UpdateTickUsed();
			return resource;
		}

		ResourceType* Get(ResourceID id)
		{
			auto it = FindResource(id);

			if (it == _resources.end())
				return nullptr;

			ResourceType* resource = &(*it);
			resource->UpdateTickUsed();
			return resource;
		}

		bool Has(ResourceID id) const
		{
			return FindResource(id) != _resources.cend();
		}

		bool Delete(ResourceID id)
		{
			auto it = FindResource(id);

			if (it == _resources.end())
				return false;

			_resources.Remove(it);
			return true;
		}

		void Invalidate()
		{
			_resources.Clear();
		}

		int PurgeStaleResources()
		{
			return _resources.RemoveAll([](const ResourceType& resource) { return resource.IsStale(); });
		}

		List<ResourceType>::Iterator begin() { return _resources.begin(); }
		List<ResourceType>::Iterator end() { return _resources.end(); }

		List<ResourceType>::ConstIterator cbegin() const { return _resources.cbegin(); }
		List<ResourceType>::ConstIterator cend() const { return _resources.cend(); }

	private:
		List<ResourceType>::Iterator FindResource(ResourceID id)
		{
			return _resources.Find([id](const ResourceType& resource) { return resource.GetID() == id; });
		}

		List<ResourceType>::ConstIterator FindResource(ResourceID id) const
		{
			return _resources.Find([id](const ResourceType& resource) { return resource.GetID() == id; });
		}
	};
}