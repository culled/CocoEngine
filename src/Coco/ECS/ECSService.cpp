//
// Created by cullen on 4/1/26.
//

#include "ECSService.h"

#include "Scene.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    ECSService::ECSService(Engine* engine) :
        EngineService(engine),
        _components(),
        _entities(&_components),
        _destroyEntityQueue(),
        _destroyEntitiesTickListener(this, &ECSService::DestroyEntityTick, DestroyEntitiesTickOrder)
    {
        _destroyEntitiesTickListener.ListenTo(*_engine->GetMainLoop());

        COCO_ENGINE_LOG_VERBOSE("Created ECSService");
    }

    ECSService::~ECSService()
    {
        _destroyEntitiesTickListener.StopListening();
        _destroyEntityQueue.Clear(true);
        _entities.Clear();

        COCO_ENGINE_LOG_VERBOSE("Destroyed ECSService");
    }

    Entity ECSService::CreateEntity(const char* name, Scene& owningScene, const UUID& parentID)
    {
        UUID entityID = _entities.Create(name, owningScene, parentID);
        return Entity(this, entityID);
    }

    bool ECSService::IsEntityValid(const UUID& entityID) const
    {
        return _entities.Has(entityID);
    }

    Entity ECSService::GetEntity(const UUID& entityID)
    {
        if (IsEntityValid(entityID))
            return Entity(this, entityID);

        return Entity();
    }

    void ECSService::SetEntityName(const UUID& entityID, const char* name)
    {
        if (auto existing = _entities.TryGet(entityID))
            existing->Name = name;
    }

    String ECSService::GetEntityName(const UUID& entityID) const
    {
        if (auto existing = _entities.TryGet(entityID))
            return existing->Name;

        return String();
    }

    void ECSService::SetEntityIsActive(const UUID& entityID, bool isActive)
    {
        if (auto existing = _entities.TryGet(entityID))
            existing->IsActive = isActive;
    }

    bool ECSService::IsEntityActive(const UUID& entityID) const
    {
        if (auto existing = _entities.TryGet(entityID))
            return existing->IsActive;

        return false;
    }

    bool ECSService::IsEntityActiveInScene(const UUID& entityID) const
    {
        if (auto existing = _entities.TryGet(entityID))
        {
            if (!existing->IsActive || existing->ParentID == UUID::Nil)
                return existing->IsActive;

            return IsEntityActiveInScene(existing->ParentID);
        }

        return false;
    }

    Scene* ECSService::GetEntityScene(const UUID& entityID)
    {
        if (auto existing = _entities.TryGet(entityID))
            return existing->OwningScene;

        return nullptr;
    }

    Scene* ECSService::GetEntityScene(const UUID& entityID) const
    {
        if (auto existing = _entities.TryGet(entityID))
            return existing->OwningScene;

        return nullptr;
    }

    bool ECSService::EntityHasParent(const UUID& entityID) const
    {
        if (auto existing = _entities.TryGet(entityID))
            return existing->ParentID != UUID::Nil;

        return false;
    }

    Entity ECSService::GetEntityParent(const UUID& entityID)
    {
        if (auto existing = _entities.TryGet(entityID))
            return Entity(this, existing->ParentID);

        return Entity();
    }

    uint64 ECSService::GetEntityChildCount(const UUID& entityID) const
    {
        if (auto existing = _entities.TryGet(entityID))
            return existing->ChildrenIDs.GetCount();

        return 0;
    }

    void ECSService::DestroyEntityImmediate(const UUID& entityID)
    {
        _entities.Remove(entityID);
    }

    void ECSService::DestroyEntity(const UUID& entityID)
    {
        _destroyEntityQueue.Append(entityID);
    }

    void ECSService::DestroyEntityTick(const TickInfo& tickInfo)
    {
        while (!_destroyEntityQueue.IsEmpty())
        {
            const auto& id = _destroyEntityQueue.Front();
            if (_entities.Has(id))
                DestroyEntityImmediate(id);

            _destroyEntityQueue.RemoveAt(0, false);
        }
    }
} // Coco