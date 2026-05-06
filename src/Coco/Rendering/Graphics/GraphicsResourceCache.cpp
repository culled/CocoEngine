//
// Created by cullen on 3/29/26.
//

#include "GraphicsResourceCache.h"

#include "Resources/Image.h"

namespace Coco
{
    GraphicsResourceCache::GraphicsResourceCache(GraphicsPlatform* platform) :
        _platform(platform),
        _images(),
        _resourcesInUse()
    {}

    GraphicsResourceCache::~GraphicsResourceCache()
    {
        _resourcesInUse.Clear(true);

        for (const auto& pair : _images)
            _platform->InvalidateResource(pair.first->GetID());

        _images.Clear(true);
    }

    Ref<Image> GraphicsResourceCache::GetOrCreateImage(const ImageDescription& imageDescription)
    {
        int64 existingIndex = _images.Find([&](const std::pair<Ref<Image>, uint64>& item)
        {
            if (_resourcesInUse.Contains(item.first->GetID()))
                return false;

            return item.first->GetDescription() == imageDescription;
        });

        if (existingIndex == -1)
        {
            existingIndex = static_cast<int64>(_images.GetCount());
            _images.EmplaceBack(_platform->CreateImage(imageDescription), 0);
        }

        auto& pair = _images[existingIndex];
        pair.second = _platform->GetCurrentFrameNumber();
        _resourcesInUse.Append(pair.first->GetID());
        return pair.first;
    }

    void GraphicsResourceCache::ReleaseResource(uint64 id)
    {
        _resourcesInUse.Remove(id, false);
    }
} // Coco