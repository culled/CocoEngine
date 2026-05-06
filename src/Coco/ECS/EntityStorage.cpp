//
// Created by cullen on 4/1/26.
//

#include "EntityStorage.h"

#include "Scene.h"

namespace Coco
{
    EntityData::EntityData(const char* name, Scene& owningScene, const UUID& parentID) :
        Name(name),
        OwningScene(&owningScene),
        ParentID(parentID),
        ChildrenIDs(),
        IsActive(true)
    {}

    EntityStorage::EntityStorage(EntityComponentStorage* componentStorage) :
        _components(componentStorage),
        _entities()
    {}

    EntityStorage::~EntityStorage()
    {
        Clear();
    }

    UUID EntityStorage::Create(const char* name, Scene& owningScene, const UUID& parentID)
    {
        UUID id = UUID::New();
        Create(id, name, owningScene, parentID);
        return id;
    }

    void EntityStorage::Create(const UUID& id, const char* name, Scene& owningScene, const UUID& parentID)
    {
        EntityData& data = _entities.Emplace(id, name, owningScene, parentID);

        if (parentID != UUID::Nil)
        {
            if (auto existing = _entities.TryGetValue(parentID))
            {
                existing->ChildrenIDs.Append(id);
            }
            else
            {
                data.ParentID = UUID::Nil;
            }
        }
        else
        {
            owningScene._rootEntities.Append(id);
        }
    }

    EntityData* EntityStorage::TryGet(const UUID& id)
    {
        return _entities.TryGetValue(id);
    }

    const EntityData* EntityStorage::TryGet(const UUID& id) const
    {
        return _entities.TryGetValue(id);
    }

    void EntityStorage::Remove(const UUID& id)
    {
        if (auto existing = _entities.TryGetValue(id))
        {
            if (existing->ParentID != UUID::Nil)
            {
                auto& parent = _entities.Get(existing->ParentID);
                parent.ChildrenIDs.Remove(id);
            }

            RemoveEntityAndChildren(id, *existing);
        }
    }

    void EntityStorage::Clear()
    {
        _components->Clear();
        _entities.Clear();
    }

    void EntityStorage::RemoveEntityAndChildren(const UUID& id, const EntityData& entity)
    {
        for (const auto& childID : entity.ChildrenIDs)
        {
            const auto& child = _entities.Get(childID);
            RemoveEntityAndChildren(childID, child);
        }

        _components->RemoveAll(id);

        if (entity.ParentID == UUID::Nil)
            entity.OwningScene->_rootEntities.Remove(id);

        _entities.Remove(id);
    }
} // Coco