//
// Created by cullen on 3/8/26.
//

#ifndef COCOENGINE_TEXTURE_H
#define COCOENGINE_TEXTURE_H
#include "Coco/Core/IO/FilePath.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Resources/Resource.h"
#include "Graphics/Resources/Image.h"
#include "Graphics/Resources/ImageSampler.h"
#include "Graphics/Resources/ImageSamplerTypes.h"

namespace Coco
{
    class Texture : public Resource
    {
        DECLARE_RTTI_TYPE(Texture)

    public:
        Texture(Engine* engine, uint64 id, const ImageDescription& imageDescription, const ImageSamplerDescription& samplerDescription);
        Texture(Engine* engine, uint64 id, const FilePath& imagePath, ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, const ImageSamplerDescription& samplerDescription, bool generateMipMaps);
        ~Texture();

        void SetPixels(const void* pixelData, uint64 pixelDataSize);
        void Resize(uint32 newWidth, uint32 newHeight);

        Ref<Image> GetImage() { return _image; }
        Ref<ImageSampler> GetSampler() { return _sampler; }

    private:
        Ref<Image> _image;
        Ref<ImageSampler> _sampler;

    private:
        static ImageSamplerDescription UpdateSamplerDescription(const ImageDescription& imageDesc, const ImageSamplerDescription& samplerDesc);
        Ref<Image> CreateImageFromFile(const FilePath& imagePath, ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, bool generateMipMaps);
    };
} // Coco

#endif //COCOENGINE_TEXTURE_H