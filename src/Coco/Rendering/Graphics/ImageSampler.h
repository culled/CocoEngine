#pragma once
#include "GraphicsResource.h"
#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
    class ImageSampler :
        public GraphicsResource
    {
    public:
        ~ImageSampler() = default;

        virtual const ImageSamplerDescription& GetDescription() const = 0;

    protected:
        ImageSampler(const GraphicsResourceID& id);
    };
}