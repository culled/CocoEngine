#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Singleton.h>
#include <Coco/Core/Types/String.h>
#include "entt.h"
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Engine.h>

namespace Coco::ECS
{
	class Entity;
	class Scene;

	class ECSService :
		public EngineService,
		public Singleton<ECSService>
	{
		//friend class Entity;
		//friend class Scene;
		template<typename BaseType, typename ... OtherTypes>
		friend class ComponentView;

	public:
		static const int EarlyTickPriority;

		Event<Entity&> OnEntityCreated;
		Event<Entity&> OnEntityParentChanged;
		Event<const Entity&> OnEntityDestroyed;

	public:
		ECSService();
		~ECSService();

		Entity CreateEntity(const string& name, const Entity& parent, const SharedRef<Scene>& scene);
		bool IsEntityValid(const Entity& entity) const;
		void QueueDestroyEntity(Entity& entity);
		void DestroyEntity(Entity& entity);

		void SetEntityParent(Entity& entity, const Entity& parent);
		void ClearEntityParent(Entity& entity);

		Entity GetEntityParent(const Entity& entity) const;
		bool IsEntityOrphaned(const Entity& entity) const;
		std::vector<Entity> GetEntityChildren(const Entity& entity) const;
		bool IsEntityDescendentOf(const Entity& entity, const Entity& ancestor) const;

		std::vector<Entity> GetSceneEntities(const Scene& scene, bool rootEntitiesOnly) const;

		template<typename ComponentType, typename ... Args>
		ComponentType& CreateComponentOnEntity(Entity& entity, Args&& ... args);

		template<typename ComponentType>
		bool EntityHasComponent(const Entity& entity) const;

		template<typename ComponentType>
		ComponentType& GetEntityComponent(const Entity& entity);

		template<typename ComponentType>
		const ComponentType& GetEntityComponent(const Entity& entity) const;

		template<typename ComponentType>
		void RemoveEntityComponent(Entity& entity);

		template<typename ComponentType>
		bool TryGetComponentEntity(const ComponentType& component, SharedRef<Scene> entityScene, Entity& outEntity);

	private:
		entt::registry _registry;
		std::unordered_map<entt::entity, entt::entity> _entityParentMap;
		std::vector<Entity> _queuedDestroyEntities;
		ManagedRef<TickListener> _earlyTickListener;

	private:
		void RegisterComponentSerializers();
		void RegisterSceneSystems();
		void RegisterResourceSerializers();
		void HandleEarlyTick(const TickInfo& tickInfo);
	};
}

#include "Entity.h"

namespace Coco::ECS
{
	template<typename ComponentType, typename ... Args>
	ComponentType& ECSService::CreateComponentOnEntity(Entity& entity, Args&& ... args)
	{
		if (EntityHasComponent<ComponentType>(entity))
		{
			CocoWarn("Entity already has a component of type {}", typeid(ComponentType).name())
			return GetEntityComponent<ComponentType>(entity);
		}

		return _registry.emplace<ComponentType>(entity._handle, entity, std::forward<Args>(args)...);
	}

	template<typename ComponentType>
	bool ECSService::EntityHasComponent(const Entity& entity) const
	{
		return _registry.all_of<ComponentType>(entity._handle);
	}

	template<typename ComponentType>
	ComponentType& ECSService::GetEntityComponent(const Entity& entity)
	{
		return _registry.get<ComponentType>(entity._handle);
	}

	template<typename ComponentType>
	const ComponentType& ECSService::GetEntityComponent(const Entity& entity) const
	{
		return _registry.get<ComponentType>(entity._handle);
	}

	template<typename ComponentType>
	void ECSService::RemoveEntityComponent(Entity& entity)
	{
		if (EntityHasComponent<ComponentType>(entity))
		{
			_registry.remove<ComponentType>(entity._handle);
		}
	}

	template<typename ComponentType>
	bool ECSService::TryGetComponentEntity(const ComponentType& component, SharedRef<Scene> entityScene, Entity& outEntity)
	{
		auto entity = entt::to_entity(_registry, component);

		if (entity == entt::null)
		{
			outEntity = Entity::Null;
			return false;
		}

		outEntity = Entity(entity, entityScene);
		return true;
	}
}