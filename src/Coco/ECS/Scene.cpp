#include "ECSpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components/EntityInfoComponent.h"
#include "Systems/SceneSystemFactory.h"
#include "Systems/SceneSystem.h"

#include <Coco/Core/Engine.h>

namespace Coco::ECS
{
	const int Scene::TickPriority = 1000;

	Scene::Scene(const ResourceID& id) :
		Resource(id),
		TickSystem<SceneTickListener, const TickInfo&>(&Scene::CompareTickListeners, &Scene::PerformTick, &Scene::HandleTickError),
		_tickListener(CreateManagedRef<TickListener>(this, &Scene::HandleTick, TickPriority)),
		_simulateMode(SceneSimulateMode::Stopped),
		_systems(SceneSystemFactory::CreateSystems(*this))
	{
		MainLoop::Get()->AddTickListener(_tickListener);
	}

	Scene::~Scene()
	{
		DestroyAllEntities(true);
		MainLoop::Get()->RemoveTickListener(_tickListener);
		_systems.clear();
	}

	Entity Scene::CreateEntity(const string& name, const Entity& parent)
	{
		return ECSService::Get()->CreateEntity(name, parent, GetSelfRef<Scene>());
	}

	std::vector<Entity> Scene::GetEntities(bool onlyRootEntities)
	{
		return ECSService::Get()->GetSceneEntities(*this, onlyRootEntities);
	}

	void Scene::QueueDestroyEntity(Entity& entity)
	{
		ECSService::Get()->QueueDestroyEntity(entity);
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		ECSService::Get()->DestroyEntity(entity);
	}

	void Scene::SetSimulateMode(SceneSimulateMode mode)
	{
		if (mode == _simulateMode)
			return;

		_simulateMode = mode;

		if (_simulateMode == SceneSimulateMode::Running)
		{
			StartSimulation();
		}
		else
		{
			StopSimulation();
		}
	}

	bool Scene::CompareTickListeners(const Ref<SceneTickListener>& a, const Ref<SceneTickListener>& b)
	{
		if (!a.IsValid())
			return false;

		if (!b.IsValid())
			return true;

		return a->Priority < b->Priority;
	}

	void Scene::PerformTick(Ref<SceneTickListener>& listener, const TickInfo& tickInfo)
	{
		listener->Tick(tickInfo);
	}

	bool Scene::HandleTickError(const std::exception& ex)
	{
		CocoError("Error during scene tick: {}", ex.what())

		return true;
	}

	void Scene::HandleTick(const TickInfo& tickInfo)
	{
		if (_simulateMode != SceneSimulateMode::Running)
			return;

		TickAllListeners(tickInfo);
	}

	void Scene::DestroyAllEntities(bool destroyImmediately)
	{
		ECSService* ecs = ECSService::Get();
		if (!ecs)
			return;

		std::vector<Entity> entities = ecs->GetSceneEntities(*this, true);

		for (auto& e : entities)
		{
			if (destroyImmediately)
			{
				ecs->DestroyEntity(e);
			}
			else
			{
				ecs->QueueDestroyEntity(e);
			}
		}
	}

	void Scene::EntitiesAdded(std::span<Entity> rootEntities)
	{
		for (const auto& s : _systems)
			s->EntitiesAdded(rootEntities);
	}

	void Scene::StartSimulation()
	{
		// Systems are sorted in ascending order of priority
		for (auto& s : _systems)
			s->SimulationStarted();
	}

	void Scene::StopSimulation()
	{	
		// End the simulation in reverse order of the simulation start
		for (auto it = _systems.rbegin(); it != _systems.rend(); ++it)
			(*it)->SimulationEnded();
	}
}