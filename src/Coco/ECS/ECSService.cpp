#include "ECSService.h"

#include "Entity.h"
#include "Scene.h"
#include <Coco/Core/Engine.h>

namespace Coco::ECS
{
	ECSService* ECSService::_instance = nullptr;

	ECSService::ECSService(EngineServiceManager* serviceManager) : EngineService(serviceManager), 
		_entities(CreateManagedRef<MappedMemoryPool<Entity, MaxEntities>>())
	{
		_instance = this;

		serviceManager->Engine->GetMainLoop()->CreateTickListener(this, &ECSService::Process, ProcessTickPriority);

		// Create the root scene
		CreateScene();
	}

	ECSService::~ECSService()
	{
		_entities.Reset();
	}

	EntityID ECSService::CreateEntity(const string& name, EntityID parentID)
	{
		uint64_t id = _entities->GetNextReserveIndex();

		if (!_entities->TryReserve(Entity(id, name, parentID), id))
			throw Exception("Cannot create more entities");

		return id;
	}

	Entity& ECSService::GetEntity(EntityID entityID)
	{
		return _entities->Get(entityID);
	}

	bool ECSService::TryGetEntity(EntityID entityID, Entity*& entity)
	{
		return _entities->TryGet(entityID, entity);
	}

	List<EntityID> ECSService::GetEntityChildrenIDs(EntityID entity)
	{
		List<EntityID> children;

		for (const auto& child : *(_entities.Get()))
		{
			if (child._parentID == entity)
				children.Add(child._id);
		}

		return children;
	}

	List<Entity*> ECSService::GetEntityChildren(EntityID entity)
	{
		List<Entity*> children;

		for (auto& entity : *(_entities.Get()))
		{
			if (entity._parentID == entity._id)
				children.Add(&entity);
		}

		return children;
	}

	Entity& ECSService::GetEntityParent(EntityID entityID)
	{
		return GetEntity(GetEntity(entityID).GetParentID());
	}

	bool ECSService::TryGetEntityParent(EntityID entityID, Entity*& entity)
	{
		Entity* child;
		if (!TryGetEntity(entityID, child))
			return false;

		return TryGetEntity(child->GetParentID(), entity);
	}

	bool ECSService::IsDescendantOfEntity(EntityID entityID, EntityID parentID)
	{
		if (entityID == parentID)
			return true;

		Entity* parent = nullptr;

		if (!TryGetEntityParent(entityID, parent))
			return false;

		return parent->GetID() == parentID || IsDescendantOfEntity(parent->GetID(), parentID);
	}

	void ECSService::QueueDestroyEntity(EntityID entityID)
	{
		_queuedEntitiesToDestroy.emplace(entityID);
	}
	
	void ECSService::DestroyEntity(EntityID entityID)
	{
		// Release the entity and any of its descendants
		for (const auto& entity : *(_entities.Get()))
		{
			if (IsDescendantOfEntity(entity._id, entityID))
				_entities->Release(entity._id);
		}
	}

	Scene* ECSService::CreateScene(const string& name, SceneID parentID)
	{
		_scenes.Add(CreateManagedRef<Scene>(_nextSceneID, name, parentID));
		_nextSceneID++;

		return _scenes.Last().Get();
	}

	Scene* ECSService::GetScene(SceneID sceneID)
	{
		const auto it = _scenes.Find([sceneID](const auto& scene) { return scene->_id == sceneID; });

		if (it == _scenes.end())
			return nullptr;

		return (*it).Get();
	}

	bool ECSService::TryGetScene(SceneID sceneID, Scene*& scene)
	{
		scene = GetScene(sceneID);
		return scene == nullptr;
	}

	bool ECSService::IsDescendantOfScene(SceneID sceneID, SceneID parentID)
	{
		if (sceneID == parentID || parentID == RootSceneID)
			return true;

		Scene* parent = GetScene(parentID);

		if (parent == nullptr)
			return false;

		return parent->GetID() == parentID || IsDescendantOfScene(parent->GetID(), parentID);
	}

	bool ECSService::IsEntityInScene(EntityID entityID, SceneID sceneID)
	{
		return IsDescendantOfScene(GetEntity(entityID).GetSceneID(), sceneID);
	}

	void ECSService::SetEntityScene(EntityID entityID, SceneID sceneID)
	{
		for (auto& entity : *(_entities.Get()))
		{
			if (IsDescendantOfEntity(entity._id, entityID))
				entity._sceneID = sceneID;
		}
	}

	void ECSService::DestroyScene(SceneID sceneID)
	{
		for (const auto& entity : *(_entities.Get()))
		{
			if (entity._sceneID == sceneID)
				QueueDestroyEntity(entity._id);
		}
	}

	List<EntityID> ECSService::GetSceneEntityIDs(SceneID scene) const
	{
		List<EntityID> sceneEntities;
		return sceneEntities;
	}

	void ECSService::Process(double deltaTime)
	{
		for (const auto& entityID : _queuedEntitiesToDestroy)
		{
			DestroyEntity(entityID);
		}

		_queuedEntitiesToDestroy.clear();
	}
}