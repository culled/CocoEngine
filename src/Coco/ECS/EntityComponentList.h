#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/SparseSet.h>
#include "EntityTypes.h"

namespace Coco::ECS
{
	class IEntityComponentList
	{
	public:
		virtual ~IEntityComponentList() = default;
	};

	template <typename ComponentType, int MaxComponents>
	class EntityComponentList : public IEntityComponentList
	{
	public:
		template<typename ... Args>
		ComponentType& AddComponent(EntityID entity, Args&& ... args)
		{
			return _components.Add(entity, ComponentType(entity, std::forward<Args>(args)...));
		}

		ComponentType& GetComponent(EntityID entity)
		{
			return _components.Get(entity);
		}

		bool HasComponent(EntityID entity) const
		{
			return _components.Contains(entity);
		}

		bool RemoveComponent(EntityID entity)
		{
			return _components.Remove(entity);
		}

	private:
		SparseSet<ComponentType, MaxComponents> _components;
	};
}