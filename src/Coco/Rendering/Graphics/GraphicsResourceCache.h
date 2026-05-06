//
// Created by cullen on 3/29/26.
//

#ifndef COCOENGINE_GRAPHICSRESOURCECACHE_H
#define COCOENGINE_GRAPHICSRESOURCECACHE_H
#include "GraphicsPlatform.h"

namespace Coco
{
    class GraphicsResourceCache
    {
    public:
        GraphicsResourceCache(GraphicsPlatform* platform);
        ~GraphicsResourceCache();

        Ref<Image> GetOrCreateImage(const ImageDescription& imageDescription);

        void ReleaseResource(uint64 id);

    private:
        GraphicsPlatform* _platform;
        Array<std::pair<Ref<Image>, uint64>> _images;
        Array<uint64> _resourcesInUse;
    };
} // Coco

#endif //COCOENGINE_GRAPHICSRESOURCECACHE_H