//
// Created by cullen on 3/22/26.
//

#include "GraphicsResourceManager.h"

namespace Coco
{
    GraphicsResourceManager::GraphicsResourceManager() :
        _resources(),
        _nextResourceID(0)
    {}

    GraphicsResourceManager::~GraphicsResourceManager()
    {
        _resources.Clear();
    }

    void GraphicsResourceManager::Invalidate(uint64 resourceID)
    {
        if (auto res = _resources.TryGetValue(resourceID))
        {
            if (res->GetUseCount() > 1)
                return;

            _resources.Remove(resourceID);
        }
    }
} // Coco