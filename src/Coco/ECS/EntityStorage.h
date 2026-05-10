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

    /// @brief Storage for entities
    class EntityStorage
    {
    public:
        EntityStorage(EntityComponentStorage* componentStorage);
        ~EntityStorage();

        /// @brief Creates a new entity and returns its ID
        /// @param name The name of the entity
        /// @param owningScene The scene that will own the entity
        /// @param parentID The ID of the entity that will parent the new entity, or UUID::Nil for the new entity to not have a parent
        /// @return The ID of the new entity
        UUID Create(const char* name, Scene& owningScene, const UUID& parentID);

        /// @brief Creates an entity with the given ID
        /// @param id The ID of the entity
        /// @param name The name of the entity
        /// @param owningScene The scene that will own the entity
        /// @param parentID The ID of the entity that will parent the new entity, or UUID::Nil for the new entity to not have a parent
        void Create(const UUID& id, const char* name, Scene& owningScene, const UUID& parentID);

        /// @brief Determines if an entity with the given ID exists
        /// @param id The ID of the entity
        /// @return True if the entity exists
        bool Has(const UUID& id) const { return _entities.Contains(id); }

        /// @brief Gets the data for an entity. NOTE: use Has() to check if the entity exists first
        /// @param id The ID of the entity
        /// @return The data for the entity
        EntityData& Get(const UUID& id) { return _entities.Get(id); }

        /// @brief Gets the data for an entity. NOTE: use Has() to check if the entity exists first
        /// @param id The ID of the entity
        /// @return The data for the entity
        const EntityData& Get(const UUID& id) const { return _entities.Get(id); }

        /// @brief Attempts to get the data for an entity if it exists
        /// @param id The ID of the entity
        /// @return The data for the entity, or nullptr if the entity does not exist
        EntityData* TryGet(const UUID& id);

        /// @brief Attempts to get the data for an entity if it exists
        /// @param id The ID of the entity
        /// @return The data for the entity, or nullptr if the entity does not exist
        const EntityData* TryGet(const UUID& id) const;

        /// @brief Removes an entity, making it invalid
        /// @param id The ID of the entity
        void Remove(const UUID& id);

        /// @brief Removes all entities
        void Clear();

    private:
        EntityComponentStorage* _components;
        Map<UUID, EntityData> _entities;

        /// @brief Removes an entity and all of its children recursively
        /// @param id The ID of the entity
        /// @param entity The entity data
        void RemoveEntityAndChildren(const UUID& id, const EntityData& entity);
    };
} // Coco

#endif //COCOENGINE_ENTITYSTORAGE_H