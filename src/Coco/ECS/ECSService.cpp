#include "ECSService.h"
#include "Entity.h"

namespace Coco::ECS
{
	ECSService* ECSService::_instance = nullptr;

	ECSService::ECSService(EngineServiceManager* serviceManager) : EngineService(serviceManager), 
		_entities(CreateManaged<MappedMemoryPool<Entity, MaxEntities>>())
	{
		_instance = this;

		RegisterTickListener(this, &ECSService::Process, ProcessTickPriority);
	}

	ECSService::~ECSService()
	{
		_entities.reset();
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

		for (const auto& entity : *(_entities.get()))
		{
			if (entity._parentID == entity)
				children.Add(entity._id);
		}

		return children;
	}

	List<Entity*> ECSService::GetEntityChildren(EntityID entity)
	{
		List<Entity*> children;

		for (auto& entity : *(_entities.get()))
		{
			if (entity._parentID == entity)
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

	void ECSService::QueueEntityDestroy(EntityID entityID)
	{
		_queuedEntitiesToDestroy.emplace(entityID);
	}

	void ECSService::Process(double deltaTime)
	{
	}
}