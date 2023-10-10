#include "ECSpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components/EntityInfoComponent.h"

#include <Coco/Core/Engine.h>

namespace Coco::ECS
{
	const int Scene::sLateTickPriority = 100000;

	Scene::Scene(const ResourceID& id, const string& name) :
		Resource(id, name),
		_lateTickListener(CreateManagedRef<TickListener>(this, &Scene::HandleLateTick, sLateTickPriority)),
		_registry(),
		_queuedDestroyEntities()
	{
		MainLoop::Get()->AddListener(_lateTickListener);
	}

	Scene::~Scene()
	{
		MainLoop::Get()->RemoveListener(_lateTickListener);
	}

	Entity Scene::CreateEntity(const string& name)
	{
		Entity e(_registry.create(), GetSelfRef<Scene>());
		e.AddComponent<EntityInfoComponent>(name, std::optional<Entity>());
		return e;
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

	void Scene::HandleLateTick(const TickInfo& tickInfo)
	{
		for (Entity& e : _queuedDestroyEntities)
			DestroyEntityImmediate(e);

		_queuedDestroyEntities.clear();
	}
}