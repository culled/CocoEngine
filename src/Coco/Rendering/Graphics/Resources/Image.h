//
// Created by cullen on 3/9/26.
//

#ifndef COCOENGINE_IMAGE_H
#define COCOENGINE_IMAGE_H
#include "ImageTypes.h"
#include "Coco/Rendering/Graphics/GraphicsResource.h"

namespace Coco
{
    class Image : public GraphicsResource
    {
    public:
        virtual ~Image() = default;

        virtual void SetPixels(const void* pixelData, uint64 pixelDataSize) = 0;

        const ImageDescription& GetDescription() const { return _description; }
        uint64 GetPixelDataSize() const;

    protected:
        ImageDescription _description;

    protected:
        Image(uint64 id, const ImageDescription& description);
    };
} // Coco

#endif //COCOENGINE_IMAGE_H