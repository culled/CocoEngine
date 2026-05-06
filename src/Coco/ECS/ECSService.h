//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_ECSSERVICE_H
#define COCOENGINE_ECSSERVICE_H
#include "Entity.h"
#include "EntityComponentStorage.h"
#include "EntityStorage.h"
#include "Coco/Core/EngineService.h"
#include "Coco/Core/ProcessLoop/TickInfo.h"
#include "Coco/Core/ProcessLoop/TickListener.h"
#include "Coco/Core/RTTI/RTTI.h"

namespace Coco
{
    class ECSService : public EngineService
    {
    public:
        static constexpr int DestroyEntitiesTickOrder = 8000;

    public:
        ECSService(Engine* engine);
        ~ECSService();

        Entity CreateEntity(const char* name, Scene& owningScene, const UUID& parentID);

        bool IsEntityValid(const UUID& entityID) const;

        Entity GetEntity(const UUID& entityID);

        void SetEntityName(const UUID& entityID, const char* name);
        String GetEntityName(const UUID& entityID) const;

        void SetEntityIsActive(const UUID& entityID, bool isActive);
        bool IsEntityActive(const UUID& entityID) const;
        bool IsEntityActiveInScene(const UUID& entityID) const;

        Scene* GetEntityScene(const UUID& entityID);
        Scene* GetEntityScene(const UUID& entityID) const;

        bool EntityHasParent(const UUID& entityID) const;
        Entity GetEntityParent(const UUID& entityID);

        uint64 GetEntityChildCount(const UUID& entityID) const;

        void DestroyEntityImmediate(const UUID& entityID);
        void DestroyEntity(const UUID& entityID);

        EntityStorage* GetEntityStorage() { return &_entities; }
        const EntityStorage* GetEntityStorage() const { return &_entities; }
        EntityComponentStorage* GetComponentStorage() { return &_components; }

    private:
        EntityComponentStorage _components;
        EntityStorage _entities;
        Array<UUID> _destroyEntityQueue;
        TickListener _destroyEntitiesTickListener;

    private:
        void DestroyEntityTick(const TickInfo& tickInfo);
    };
} // Coco

#endif //COCOENGINE_ECSSERVICE_H