//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_ENTITYSTORAGE_H
#define COCOENGINE_ENTITYSTORAGE_H
#include "EntityComponentStorage.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Core/Types/Map.h"
#include "Coco/Core/Types/String.h"
#include "Coco/Core/Types/UUID.h"

namespace Coco
{
    class Scene;

    /// @brief Data for an Entity
    struct EntityData
    {
        /// @brief The Entity's name
        String Name;

        /// @brief The scene that the entity exists in
        Scene* OwningScene;

        /// @brief The ID of the Entity's parent
        UUID ParentID;

        /// @brief The IDs of the Entity's children
        Array<UUID> ChildrenIDs;

        /// @brief The active state of the Entity
        bool IsActive;

        EntityData(const char* name, Scene& owningScene, const UUID& parentID);
    };

    class EntityStorage
    {
    public:
        EntityStorage(EntityComponentStorage* componentStorage);
        ~EntityStorage();

        UUID Create(const char* name, Scene& owningScene, const UUID& parentID);
        void Create(const UUID& id, const char* name, Scene& owningScene, const UUID& parentID);
        bool Has(const UUID& id) const { return _entities.Contains(id); }
        EntityData& Get(const UUID& id) { return _entities.Get(id); }
        const EntityData& Get(const UUID& id) const { return _entities.Get(id); }

        EntityData* TryGet(const UUID& id);
        const EntityData* TryGet(const UUID& id) const;

        void Remove(const UUID& id);
        void Clear();

    private:
        EntityComponentStorage* _components;
        Map<UUID, EntityData> _entities;

        void RemoveEntityAndChildren(const UUID& id, const EntityData& entity);
    };
} // Coco

#endif //COCOENGINE_ENTITYSTORAGE_H