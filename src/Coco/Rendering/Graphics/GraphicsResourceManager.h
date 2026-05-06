//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_GRAPHICSRESOURCEMANAGER_H
#define COCOENGINE_GRAPHICSRESOURCEMANAGER_H
#include "GraphicsResource.h"
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Types/Map.h"

namespace Coco
{
    class GraphicsResourceManager
    {
    public:
        GraphicsResourceManager();
        ~GraphicsResourceManager();

        template<typename ResourceType, typename ... Args>
        Ref<ResourceType> Create(Args&& ... args)
        {
            uint64 id = _nextResourceID++;
            ManagedRef<GraphicsResource>& res = _resources.Emplace(id, CreateDefaultManagedRef<ResourceType>(id, std::forward<Args>(args)...));
            return res.DowncastAsRef<ResourceType>();
        }

        void Invalidate(uint64 resourceID);
    private:
        Map<uint64, ManagedRef<GraphicsResource>> _resources;
        uint64 _nextResourceID;
    };
} // Coco

#endif //COCOENGINE_GRAPHICSRESOURCEMANAGER_H