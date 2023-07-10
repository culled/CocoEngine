#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/SparseSet.h>
#include "EntityTypes.h"

#include <type_traits>

namespace Coco::ECS
{
	class IEntityComponentList
	{
	public:
		virtual ~IEntityComponentList() = default;

		virtual bool HasComponent(EntityID entityID) const = 0;
		virtual bool RemoveComponent(EntityID entityID) = 0;
	};

	template <typename ComponentType>
	class EntityComponentList : public IEntityComponentList
	{
	private:
		UnorderedMap<uint64_t, ComponentType> _components;

	public:
		template<typename ... Args>
		ComponentType& AddComponent(EntityID entity, Args&& ... args)
		{
			auto result = _components.try_emplace(entity, entity, std::forward<Args>(args)...).first;
			return result->second;
		}

		ComponentType& GetComponent(EntityID entity)
		{
			return _components.at(entity);
		}

		bool HasComponent(EntityID entity) const final
		{
			return _components.contains(entity);
		}

		bool RemoveComponent(EntityID entity) final
		{
			return _components.erase(entity);
		}
	};
}