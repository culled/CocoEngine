#include "ECSService.h"

#include "Entity.h"
#include "Scene.h"
#include <Coco/Core/Engine.h>

namespace Coco::ECS
{
	ECSService::ECSService() : EngineService()
	{
		this->SetSingleton(this);

		Engine::Get()->GetMainLoop()->CreateTickListener(this, &ECSService::Process, ProcessTickPriority);

		// Create the root scene
		CreateScene();
	}

	ECSService::~ECSService()
	{
		_entities.clear();
	}

	EntityID ECSService::CreateEntity(const string& name, const EntityID& parentID)
	{
		EntityID id = GetNextEntityID();
		_entities.try_emplace(id, id, name, RootSceneID, parentID);
		return id;
	}

	Entity& ECSService::GetEntity(const EntityID& entityID)
	{
		return _entities.at(entityID);
	}

	bool ECSService::TryGetEntity(const EntityID& entityID, Entity*& entity)
	{
		if (!_entities.contains(entityID))
			return false;

		entity = &GetEntity(entityID);
		return true;
	}

	List<EntityID> ECSService::GetEntityChildrenIDs(const EntityID& entity)
	{
		List<EntityID> children;

		for (const auto& kvp : _entities)
		{
			if (kvp.second._parentID == entity)
				children.Add(kvp.second.ID);
		}

		return children;
	}

	List<Entity*> ECSService::GetEntityChildren(const EntityID& entity)
	{
		List<Entity*> children;

		for (auto& kvp : _entities)
		{
			if (kvp.second._parentID == entity)
				children.Add(&kvp.second);
		}

		return children;
	}

	Entity& ECSService::GetEntityParent(const EntityID& entityID)
	{
		return GetEntity(GetEntity(entityID).GetParentID());
	}

	bool ECSService::TryGetEntityParent(const EntityID& entityID, Entity*& entity)
	{
		Entity* child;
		if (!TryGetEntity(entityID, child))
			return false;

		return TryGetEntity(child->GetParentID(), entity);
	}

	bool ECSService::IsDescendantOfEntity(const EntityID& entityID, const EntityID& parentID)
	{
		if (entityID == parentID)
			return true;

		Entity* parent = nullptr;

		if (!TryGetEntityParent(entityID, parent))
			return false;

		return parent->ID == parentID || IsDescendantOfEntity(parent->ID, parentID);
	}

	void ECSService::QueueDestroyEntity(const EntityID& entityID)
	{
		_queuedEntitiesToDestroy.emplace(entityID);
	}
	
	void ECSService::DestroyEntity(const EntityID& entityID)
	{
		auto it = _entities.begin();

		// Release the entity and any of its descendants
		while(it != _entities.end())
		{
			if (IsDescendantOfEntity(it->second.ID, entityID))
			{
				// Destroy the entity's components
				for (auto& kvp : _componentLists)
				{
					if (kvp.second->Has(entityID))
						kvp.second->Remove(entityID);
				}

				it = _entities.erase(it);
			}
			else
				it++;
		}
	}

	List<Entity*> ECSService::GetEntities()
	{
		List<Entity*> entities;

		for (const auto& kvp : _entities)
		{
			entities.Add(&GetEntity(kvp.first));
		}

		return entities;
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

	bool ECSService::IsEntityInScene(const EntityID& entityID, SceneID sceneID)
	{
		return IsDescendantOfScene(GetEntity(entityID).GetSceneID(), sceneID);
	}

	void ECSService::SetEntityScene(const EntityID& entityID, SceneID sceneID)
	{
		for (auto& kvp : _entities)
		{
			if (IsDescendantOfEntity(kvp.second.ID, entityID))
				kvp.second._sceneID = sceneID;
		}
	}

	void ECSService::DestroyScene(SceneID sceneID)
	{
		for (const auto& kvp : _entities)
		{
			if (kvp.second._sceneID == sceneID)
				QueueDestroyEntity(kvp.second.ID);
		}
	}

	List<EntityID> ECSService::GetSceneEntityIDs(SceneID scene) const
	{
		List<EntityID> sceneEntities;
		return sceneEntities;
	}

	void ECSService::Process(double deltaTime)
	{
		GetRootScene()->Tick(deltaTime);

		for (const auto& entityID : _queuedEntitiesToDestroy)
		{
			DestroyEntity(entityID);
		}

		_queuedEntitiesToDestroy.clear();
	}

	EntityID ECSService::GetNextEntityID()
	{
		return UUID::CreateV4();
	}
}