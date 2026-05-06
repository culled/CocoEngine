//
// Created by cullen on 3/7/26.
//

#include "ResourceManager.h"

namespace Coco
{
    ResourceManager::ResourceManager(Engine* engine) :
        _engine(engine),
        _resources()
    {}

    bool ResourceManager::HasResource(uint64 id) const noexcept
    {
        return _resources.Contains(id);
    }

    SharedPtr<Resource> ResourceManager::GetResource(uint64 id)
    {
        return _resources.Get(id);
    }

    void ResourceManager::RemoveResource(uint64 id) noexcept
    {
        _resources.Remove(id);
    }
} // Coco