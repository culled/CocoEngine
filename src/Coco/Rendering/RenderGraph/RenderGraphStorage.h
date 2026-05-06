//
// Created by cullen on 3/9/26.
//

#ifndef COCOENGINE_RENDERGRAPHSTORAGE_H
#define COCOENGINE_RENDERGRAPHSTORAGE_H
#include "RenderGraphTypes.h"
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/Types/Map.h"
#include "Coco/Rendering/Graphics/Resources/Image.h"

namespace Coco
{
    class RenderGraphStorage
    {
    public:
        void AddExternalTexture(uint32 id, Ref<Image> image);
        bool HasImage(uint32 id) const { return _images.Contains(id); }
        Ref<Image> GetImage(const RenderGraphTextureResource& texture);

    private:
        Map<uint32, Ref<Image>> _images;
    };
} // Coco

#endif //COCOENGINE_RENDERGRAPHSTORAGE_H