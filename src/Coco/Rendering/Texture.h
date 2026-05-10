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
    /// @brief Holds pixel data used during rendering
    class Texture : public Resource
    {
        DECLARE_RTTI_TYPE(Texture)

    public:
        Texture(Engine* engine, uint64 id, const ImageDescription& imageDescription, const ImageSamplerDescription& samplerDescription);
        Texture(Engine* engine, uint64 id, const FilePath& imagePath, ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, const ImageSamplerDescription& samplerDescription, bool generateMipMaps);
        ~Texture();

        /// @brief Sets the pixel data of this texture
        /// @param pixelData The raw pixel data
        /// @param pixelDataSize The size of the raw pixel data
        void SetPixels(const void* pixelData, uint64 pixelDataSize);

        /// @brief Changes the size of this texture
        /// @param newWidth The new width, in pixels
        /// @param newHeight The new height, in pixels
        void Resize(uint32 newWidth, uint32 newHeight);

        /// @brief Gets the image data resource
        /// @return The image data resource
        Ref<Image> GetImage() { return _image; }

        /// @brief Gets the image sampler resource
        /// @return The image sampler resource
        Ref<ImageSampler> GetSampler() { return _sampler; }

    private:
        Ref<Image> _image;
        Ref<ImageSampler> _sampler;

        /// @brief Updates an image sampler description to match limits of the given image description
        /// @param imageDesc The image description
        /// @param samplerDesc The image sampler description
        /// @return The updated image sampler description
        static ImageSamplerDescription UpdateSamplerDescription(const ImageDescription& imageDesc, const ImageSamplerDescription& samplerDesc);

        /// @brief Creates an image resource from an image file
        /// @param imagePath The path to the image file
        /// @param pixelFormat The image file's pixel format
        /// @param colorSpace The image file's color space
        /// @param generateMipMaps If true, mip maps will be generated
        /// @return The image
        Ref<Image> CreateImageFromFile(const FilePath& imagePath, ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, bool generateMipMaps);
    };
} // Coco

#endif //COCOENGINE_TEXTURE_H