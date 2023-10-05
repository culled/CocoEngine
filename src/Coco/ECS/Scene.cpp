#include "ECSpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components/EntityInfoComponent.h"

namespace Coco::ECS
{
	Scene::Scene() :
		_registry()
	{}

	Scene::~Scene()
	{
	}

	SharedRef<Scene> Scene::Create()
	{
		return SharedRef<Scene>(new Scene());
	}

	Entity Scene::CreateEntity(const string& name)
	{
		Entity e(_registry.create(), shared_from_this());
		e.AddComponent<EntityInfoComponent>(name, std::optional<Entity>());
		return e;
	}

	void Scene::EachEntity(std::function<void(Entity&)> callback)
	{
		for (entt::entity e : _registry.view<entt::entity>())
		{
			Entity entity(e, shared_from_this());
			callback(entity);
		}
	}
}