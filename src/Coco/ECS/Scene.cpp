#include "ECSpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components/EntityInfoComponent.h"
#include "Components/TransformSystem.h"
#include "Components/NativeScriptSystem.h"

#include <Coco/Core/Engine.h>

namespace Coco::ECS
{
	const int Scene::sTickPriority = -5;
	const int Scene::sLateTickPriority = 1000;

	Scene::Scene(const ResourceID& id, const string& name) :
		Resource(id, name),
		_tickListener(CreateManagedRef<TickListener>(this, &Scene::HandleTick, sTickPriority)),
		_lateTickListener(CreateManagedRef<TickListener>(this, &Scene::HandleLateTick, sLateTickPriority)),
		_registry(),
		_queuedDestroyEntities(),
		_systemsNeedSorting(false)
	{
		MainLoop::Get()->AddListener(_tickListener);
		MainLoop::Get()->AddListener(_lateTickListener);
	}

	Scene::~Scene()
	{
		_systems.clear();
		Clear();
		MainLoop::Get()->RemoveListener(_tickListener);
		MainLoop::Get()->RemoveListener(_lateTickListener);
	}

	SharedRef<Scene> Scene::CreateWithDefaultSystems(const string& name)
	{
		SharedRef<Scene> scene = Engine::Get()->GetResourceLibrary().Create<Scene>(name);
		scene->UseDefaultSystems();

		return scene;
	}

	Entity Scene::CreateEntity(const string& name)
	{
		return CreateEntity(name, _registry.view<entt::entity>().size(), Entity::Null);
	}

	bool Scene::TryGetEntity(const EntityID& id, Entity& outEntity)
	{
		auto view = _registry.view<EntityInfoComponent>();

		for (auto e : view)
		{
			EntityInfoComponent& info = _registry.get<EntityInfoComponent>(e);
			if (info.GetEntityID() == id)
			{
				outEntity = Entity(e, GetSelfRef<Scene>());
				return true;
			}
		}

		return false;
	}

	void Scene::ReparentEntity(const EntityID& entityID, const EntityID& parentID)
	{
		if (entityID == InvalidEntityID)
			return;

		if (parentID == InvalidEntityID)
		{
			_entityParentMap.erase(entityID);
		}
		else
		{
			_entityParentMap[entityID] = parentID;
		}

		Entity e;
		Assert(TryGetEntity(entityID, e))
		OnEntityParentChanged.Invoke(e);
	}

	std::vector<Entity> Scene::GetRootEntities()
	{
		auto view = _registry.view<EntityInfoComponent>();

		std::vector<Entity> rootEntities;

		for (auto e : view)
		{
			const EntityInfoComponent& info = _registry.get<EntityInfoComponent>(e);

			if (_entityParentMap.contains(info.GetEntityID()))
				continue;

			rootEntities.push_back(Entity(e, GetSelfRef<Scene>()));
		}

		return rootEntities;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		_queuedDestroyEntities.push_back(entity);
	}

	void Scene::DestroyEntityImmediate(Entity& entity)
	{
		if (!entity.IsValid())
		{
			CocoError("Failed to destroy entity: entity was not valid")
			return;
		}

		if (entity._scene.lock().get() != this)
		{
			CocoError("Failed to destroy entity: entity was created in a different scene")
			return;
		}

		// Recursively destroy all children
		std::vector<Entity> children = entity.GetChildren();

		for (Entity& child : children)
			DestroyEntityImmediate(child);

		// Remove this entity from the parent-child map, if it is contained there
		EntityID id = entity.GetID();
		_entityParentMap.erase(id);

		// Actaully destroy the entity
		_registry.destroy(entity._handle);
		entity = Entity::Null;
	}

	void Scene::EachEntity(std::function<void(Entity&)> callback)
	{
		for (entt::entity e : _registry.view<entt::entity>())
		{
			// Invalid entities get read in the view, so only give out valid ones
			if (!_registry.valid(e))
				continue;

			Entity entity(e, GetSelfRef<Scene>());
			callback(entity);
		}
	}

	void Scene::UseDefaultSystems()
	{
		UseSystem<TransformSystem>();
		UseSystem<NativeScriptSystem>();
	}

	void Scene::SortSystems()
	{
		std::sort(_systems.begin(), _systems.end(), [](const UniqueRef<SceneSystem>& a, const UniqueRef<SceneSystem>& b)
			{
				return a->GetPriority() < b->GetPriority();
			});

		_systemsNeedSorting = false;
	}

	Entity Scene::CreateEntity(const string& name, const EntityID& id, const Entity& parent)
	{
		Entity e(_registry.create(), GetSelfRef<Scene>());
		e.AddComponent<EntityInfoComponent>(name, id);

		if (parent != Entity::Null)
			e.SetParent(parent);

		return e;
	}

	void Scene::HandleTick(const TickInfo& tickInfo)
	{
		if (_systemsNeedSorting)
			SortSystems();

		for (auto& s : _systems)
			s->Tick();
	}

	void Scene::HandleLateTick(const TickInfo& tickInfo)
	{
		for (Entity& e : _queuedDestroyEntities)
			DestroyEntityImmediate(e);

		_queuedDestroyEntities.clear();
	}

	void Scene::Clear()
	{
		_entityParentMap.clear();
		_queuedDestroyEntities.clear();
		
		for (const auto& e : _registry.view<entt::entity>())
		{
			_registry.destroy(e);
		}
	}
}