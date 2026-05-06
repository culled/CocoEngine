//
// Created by cullen on 3/28/26.
//

#ifndef COCOENGINE_IMAGESAMPLER_H
#define COCOENGINE_IMAGESAMPLER_H
#include "Coco/Rendering/Graphics/GraphicsResource.h"

namespace Coco
{
    class ImageSampler : public GraphicsResource
    {
    public:
        virtual ~ImageSampler() = default;

    protected:
        ImageSampler(uint64 id);
    };
} // Coco

#endif //COCOENGINE_IMAGESAMPLER_H