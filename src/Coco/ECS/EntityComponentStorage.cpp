//
// Created by cullen on 4/2/26.
//

#include "EntityComponentStorage.h"

namespace Coco
{
    bool EntityComponentStorage::Exists(const UUID& entityID, const ClassRTTI& componentType) const
    {
        for (const auto& it : _registries)
        {
            if (it.first.Is(componentType))
            {
                if (it.second->Has(entityID))
                    return true;
            }
        }

        return false;
    }

    EntityComponent* EntityComponentStorage::Get(const UUID& entityID, const ClassRTTI& componentType)
    {
        for (auto& it : _registries)
        {
            if (it.first.Is(componentType))
            {
                if (it.second->Has(entityID))
                    return it.second->Get(entityID);
            }
        }

        return nullptr;
    }

    const EntityComponent* EntityComponentStorage::Get(const UUID& entityID, const ClassRTTI& componentType) const
    {
        for (const auto& it : _registries)
        {
            if (it.first.Is(componentType))
            {
                if (it.second->Has(entityID))
                    return it.second->Get(entityID);
            }
        }

        return nullptr;
    }

    void EntityComponentStorage::Remove(const UUID& entityID, const ClassRTTI& componentType)
    {
        for (const auto& it : _registries)
        {
            if (it.first.Is(componentType))
            {
                if (it.second->Has(entityID))
                {
                    it.second->Remove(entityID);
                    return;
                }
            }
        }
    }

    void EntityComponentStorage::RemoveAll(const UUID& entityID)
    {
        for (auto& it : _registries)
        {
            BaseEntityComponentRegistry* registry = it.second.get();
            if (registry->Has(entityID))
                registry->Remove(entityID);
        }
    }

    void EntityComponentStorage::Clear()
    {
        _registries.Clear();
    }
} // Coco