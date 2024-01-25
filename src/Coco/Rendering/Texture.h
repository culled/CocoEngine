#pragma once
#include <Coco/Core/Resources/Resource.h>
#include "Graphics/Image.h"
#include "Graphics/ImageSampler.h"
#include <Coco/Core/IO/FilePath.h>

namespace Coco::Rendering
{
    class Texture :
        public Resource
    {
        friend class TextureSerializer;

    public:
        Texture(const ResourceID& id);

        Texture(
            const ResourceID& id,
            const ImageDescription& imageDescription,
            const ImageSamplerDescription& samplerDescription
        );

        Texture(
            const ResourceID& id,
            const FilePath& imageFilePath,
            ImageColorSpace colorSpace,
            ImageUsageFlags usageFlags,
            const ImageSamplerDescription& samplerDescription
        );

        ~Texture();

        // Inherited via Resource
        const std::type_info& GetType() const override { return typeid(Texture); }
        const char* GetTypename() const override { return "Texture"; }

        /// @brief Gets the image
        /// @return The image
        Ref<Image> GetImage() const { return _image; }

        /// @brief Gets the image sampler
        /// @return The image sampler
        Ref<ImageSampler> GetImageSampler() const { return _sampler; }

        /// @brief Sets the pixel data for this texture
        /// @param offset The offset in the texture memory to start loading pixel data into
        /// @param pixelData The pixel data
        /// @param pixelDataSize The size of the pixel data
        void SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize);

        /// @brief Sets the pixels of this texture
        /// @tparam DataType The type of pixel data
        /// @param offset The offset in the image buffer 
        /// @param data The pixel data
        template<typename DataType>
        void SetPixels(uint64 offset, std::span<const DataType> data)
        {
            SetPixels(offset, data.data(), data.size() * sizeof(DataType));
        }

        /// @brief Reads the pixel at the given position
        /// @param pixelCoordinate The pixel coordinate
        /// @param outData The destination for the pixel data copy. This should match the expected pixel data type
        /// @param dataSize The size of the outData structure
        //void ReadPixel(const Vector2Int& pixelCoordinate, void* outData, size_t dataSize);

        /// @brief Gets the value of a pixel at the given position 
        /// @tparam DataType The type of pixel data
        /// @param pixelCoords The coordinates of the pixel
        /// @return The pixel value
        //template<typename DataType>
        //DataType ReadPixel(const Vector2Int& pixelCoords)
        //{
        //    DataType r{};
        //    ReadPixel(pixelCoords, &r, sizeof(DataType));
        //    return r;
        //}

        /// @brief Reload this texture's image from the image file
        void ReloadImage();

    private:
        Ref<Image> _image;
        Ref<ImageSampler> _sampler;
        FilePath _imageFilePath;
        ImageDescription _imageDescription;

    private:
        static void AddTextureImageUsageFlags(ImageDescription& description);

        /// @brief Loads an image into this texture
        /// @param filePath The image file path
        /// @param colorSpace The image's color space
        /// @param usageFlags The image's usage flags
        void LoadImage(const FilePath& filePath);

        /// @brief Creates this texture's sampler
        /// @param samplerDescription The description for the sampler
        void CreateSampler(const ImageSamplerDescription& samplerDescription);
    };
}