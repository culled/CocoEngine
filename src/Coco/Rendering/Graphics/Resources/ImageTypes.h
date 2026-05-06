//
// Created by cullen on 3/9/26.
//

#ifndef COCOENGINE_IMAGETYPES_H
#define COCOENGINE_IMAGETYPES_H
#include <Coco/Core/Types/CoreTypes.h>
#include <Coco/Core/Types/EnumTypes.h>

#include "Coco/Core/Types/Color.h"

namespace Coco
{
    /// @brief Image pixel formats
    enum class ImagePixelFormat : uint8
    {
        /// @brief R, G, B, and A channels, each storing a single unsigned byte (normalized floating-point values in the shader)
        RGBA8,

        /// @brief B, G, R, and A channels, each storing a single unsigned byte (normalized floating-point values in the shader)
        BGRA8,

        /// @brief R channel storing a 32 bit signed integer
        R32_Int,

        /// @brief R and G channels, each storing 32 bit signed integers
        R32G32_Int,

        /// @brief R, G, and B channels, each storing 32 bit signed integers
        R32G32B32_Int,

        /// @brief R, G, B, and A channels, each storing 32 bit signed integers
        R32G32B32A32_Int,

        /// @brief R channel storing a 32-bit unsigned integer
        R32_UInt,

        /// @brief R and G channels, each storing 32 bit unsigned integers
        R32G32_UInt,

        /// @brief R, G, and B channels, each storing 32 bit unsigned integers
        R32G32B32_UInt,

        /// @brief R, G, B, and A channels, each storing 32 bit unsigned integers
        R32G32B32A32_UInt,

        /// @brief 24 bit depth channel with an 8 bit stencil channel
        Depth24_Stencil8,

        /// @brief 32 bit depth channel with an 8 bit stencil channel
        Depth32_Stencil8,

        /// @brief An unknown format
        Unknown
    };

    enum class ImagePixelType : uint8
    {
        Float,
        Int,
        UInt
    };

    /// @brief Image color spaces
    enum class ImageColorSpace : uint8
    {
        /// @brief sRGB color space
        sRGB,

        /// @brief Linear color space
        Linear,

        /// @brief Unknown color space
        Unknown
    };

    enum class ImageAttachmentType : uint8
    {
        Color,
        Depth,
        Stencil,
        DepthStencil,
    };

    enum class ImageUsageFlags : uint8
    {
        None = 0,
        Sampled = 1 << 0,
        RenderTarget = 1 << 1,
        TransferSource = 1 << 2,
        TransferDestination = 1 << 3,
        Presented = 1 << 4,
        HostVisible = 1 << 5
    };

    /// @brief Options for MSAA samples
    enum class MSAASamples : uint8
    {
        /// @brief One sample
        One = 0,

        /// @brief Two samples
        Two,

        /// @brief Four samples
        Four,

        /// @brief Eight samples
        Eight,

        /// @brief Sixteen samples
        Sixteen,

        /// @brief Thirty two samples
        ThirtyTwo,

        /// @brief Sixty four samples
        SixtyFour
    };

    EnumFlagOperators(ImageUsageFlags);

    struct ImageDescription
    {
        static const ImageDescription None;

        /// @brief The pixel width of the image
        uint32 Width;

        /// @brief The pixel height of the image
        uint32 Height;

        /// @brief The pixel depth of the image. Values more than 1 indicate this image is 3D
        uint32 Depth;

        /// @brief The number of layers in this image
        uint32 Layers;

        /// @brief The number of mip maps this image has
        uint8 MipCount;

        /// @brief The pixel format of this image
        ImagePixelFormat PixelFormat;

        /// @brief The color space of this image
        ImageColorSpace ColorSpace;

        /// @brief Usage flags for this image
        ImageUsageFlags UsageFlags;

        /// @brief The number of MSAA samples this image supports
        MSAASamples SampleCount;

        ImageAttachmentType AttachmentType;
        ImagePixelType PixelType;

        /// @brief Calculates the number of mip maps for an image with the given dimensions
        /// @param width The image width
        /// @param height The image height
        /// @return The number of mip map levels, including the original image
        static uint32 CalculateMipMapCount(uint32 width, uint32 height) noexcept;

        /// @brief Gets the number of bytes per pixel for a pixel format
        /// @param format The pixel format
        /// @return The number of bytes per pixel
        static uint8 GetBytesPerPixel(ImagePixelFormat format) noexcept;

        /// @brief Gets the number of channels for a pixel format
        /// @param format The pixel format
        /// @return The number of channels
        static uint8 GetChannelCount(ImagePixelFormat format) noexcept;

        static ImageAttachmentType GetAttachmentType(ImagePixelFormat format) noexcept;
        static ImagePixelType GetPixelType(ImagePixelFormat format) noexcept;

        static ImageDescription Create2D(uint32 width, uint32 height, ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, ImageUsageFlags usageFlags, bool mipMaps);

        ImageDescription();
        ImageDescription(uint32 width, uint32 height, ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, ImageUsageFlags usageFlags, bool mipMaps);
    };

    bool operator==(const ImageDescription& a, const ImageDescription& b);

    struct RenderTargetClearValue
    {
        struct DepthStencilValue
        {
            float Depth;
            uint8 Stencil;
        };

        union
        {
            float FloatColor[4];
            int IntColor[4];
            uint32 UIntColor[4];
            DepthStencilValue DepthStencil;
        };

        RenderTargetClearValue();
        RenderTargetClearValue(const Color& clearColor);
        RenderTargetClearValue(const Vector4& clearValues);
        RenderTargetClearValue(const Vector4i& clearValues);
        RenderTargetClearValue(float depth, uint8 stencil);
    };
}
#endif //COCOENGINE_IMAGETYPES_H