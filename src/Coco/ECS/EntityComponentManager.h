#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include "EntityData.h"
#include <type_traits>

namespace Coco::ECS
{
	class IEntityComponentList
	{
	public:
		virtual ~IEntityComponentList() = default;
	};

	template <typename ComponentType>
	class EntityComponentList : public IEntityComponentList
	{
	public:
		template<typename ... Args>
		ComponentType* AddComponent(EntityID entity, Args&& ... args)
		{
			auto result = _components.try_emplace(entity, entity, std::forward(args)...);
			return &((*result.first).second);
		}

		ComponentType* GetComponent(EntityID entity)
		{
			return &_components.at(entity);
		}

		bool HasComponent(EntityID entity) const
		{
			return _components.contains(entity);
		}

		void RemoveComponent(EntityID entity)
		{
			_components.erase(entity);
		}

		List<EntityID> GetEntities() 
		{ 
			List<EntityID> entities;

			for (auto& component : _components)
				entities.Add(component.second.Owner);

			return entities;
		}

	private:
		Map<EntityID, ComponentType> _components;
	};

	class EntityComponentManager
	{
	private:
		static EntityComponentManager* _instance;
		UnorderedMap<size_t, Managed<IEntityComponentList>> _componentLists;

	public:
		EntityComponentManager();

		static EntityComponentManager* Get() { return _instance; }

		template<typename ComponentType, typename ... Args>
		ComponentType* AddComponent(EntityID entity, Args&& ... args)
		{
			return GetComponentList<ComponentType>()->AddComponent(entity, std::forward(args)...);
		}

		template<typename ComponentType>
		ComponentType* GetComponent(EntityID entity)
		{
			return GetComponentList<ComponentType>()->GetComponent(entity);
		}

		template<typename ComponentType>
		bool HasComponent(EntityID entity)
		{
			return GetComponentList<ComponentType>()->HasComponent(entity);
		}

		template<typename ComponentType>
		void RemoveComponent(EntityID entity)
		{
			GetComponentList<ComponentType>()->RemoveComponent(entity);
		}

		template<typename ComponentType>
		List<EntityID> GetEntitiesWithComponent()
		{
			return GetComponentList<ComponentType>()->GetEntities();
		}

	private:
		template<typename ComponentType>
		EntityComponentList<ComponentType>* GetComponentList()
		{
			size_t hash = typeid(ComponentType).hash_code();

			if (!_componentLists.contains(hash))
				_componentLists.emplace(hash, CreateManaged<EntityComponentList<ComponentType>>());

			return static_cast<EntityComponentList<ComponentType>*>(_componentLists.at(hash).get());
		}
	};
}