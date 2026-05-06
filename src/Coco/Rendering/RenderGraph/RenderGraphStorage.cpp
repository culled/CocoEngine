//
// Created by cullen on 3/9/26.
//

#include "RenderGraphStorage.h"

namespace Coco
{
    void RenderGraphStorage::AddExternalTexture(uint32 id, Ref<Image> image)
    {
        _images.Add(id, image);
    }

    Ref<Image> RenderGraphStorage::GetImage(const RenderGraphTextureResource& texture)
    {
        if (!_images.Contains(texture.ID))
            throw Exception("Resource creation hasn't been implemented yet");

        return _images.Get(texture.ID);
    }
} // Coco