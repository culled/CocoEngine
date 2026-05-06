//
// Created by cullen on 3/9/26.
//
#include "ImageTypes.h"

#include <cstring>

#include "Coco/Core/Asserts.h"

namespace Coco
{
    const ImageDescription ImageDescription::None = ImageDescription();

    ImageDescription ImageDescription::Create2D(uint32 width, uint32 height, ImagePixelFormat pixelFormat,
        ImageColorSpace colorSpace, ImageUsageFlags usageFlags, bool mipMaps)
    {
        return ImageDescription(width, height, pixelFormat, colorSpace, usageFlags, mipMaps);
    }

    ImageDescription::ImageDescription() :
        Width(0),
        Height(0),
        Depth(0),
        Layers(0),
        MipCount(0),
        PixelFormat(ImagePixelFormat::Unknown),
        ColorSpace(ImageColorSpace::Unknown),
        UsageFlags(ImageUsageFlags::None),
        SampleCount(MSAASamples::One),
        AttachmentType(ImageAttachmentType::Color),
        PixelType(ImagePixelType::Float)
    {}

    ImageDescription::ImageDescription(uint32 width, uint32 height, ImagePixelFormat pixelFormat,
    ImageColorSpace colorSpace, ImageUsageFlags usageFlags, bool mipMaps) :
        Width(width),
        Height(height),
        Depth(1),
        Layers(1),
        MipCount(mipMaps ? CalculateMipMapCount(width, height) : 1),
        PixelFormat(pixelFormat),
        ColorSpace(colorSpace),
        UsageFlags(usageFlags),
        SampleCount(MSAASamples::One),
        AttachmentType(GetAttachmentType(pixelFormat)),
        PixelType(GetPixelType(pixelFormat))
    {}

    bool operator==(const ImageDescription& a, const ImageDescription& b)
    {
        return a.Width == b.Width &&
            a.Height == b.Height &&
            a.Depth == b.Depth &&
            a.Layers == b.Layers &&
            a.PixelFormat == b.PixelFormat &&
            a.ColorSpace == b.ColorSpace &&
            a.MipCount == b.MipCount &&
            a.SampleCount == b.SampleCount &&
            a.UsageFlags == b.UsageFlags;
    }


    uint32 ImageDescription::CalculateMipMapCount(uint32 width, uint32 height) noexcept
    {
        return static_cast<uint32>(Math::Floor(Math::Log2(static_cast<double>(Math::Max(width, height))))) + 1;
    }

    uint8 ImageDescription::GetBytesPerPixel(ImagePixelFormat format) noexcept
    {
        switch (format)
        {
            case ImagePixelFormat::RGBA8:
            case ImagePixelFormat::R32_Int:
            case ImagePixelFormat::R32_UInt:
                return 4;
            case ImagePixelFormat::R32G32_Int:
            case ImagePixelFormat::R32G32_UInt:
                return 4 * 2;
            case ImagePixelFormat::R32G32B32_Int:
            case ImagePixelFormat::R32G32B32_UInt:
                return 4 * 3;
            case ImagePixelFormat::R32G32B32A32_Int:
            case ImagePixelFormat::R32G32B32A32_UInt:
                return 4 * 4;
            case ImagePixelFormat::Depth24_Stencil8:
                return 4;
            default:
                COCO_ASSERT(false, "Unsupported format: %d", static_cast<int>(format));
                return 0;
        }
    }

    uint8 ImageDescription::GetChannelCount(ImagePixelFormat format) noexcept
    {
        switch (format)
        {
            case ImagePixelFormat::R32_Int:
            case ImagePixelFormat::R32_UInt:
                return 1;
            case ImagePixelFormat::R32G32_Int:
            case ImagePixelFormat::R32G32_UInt:
            case ImagePixelFormat::Depth24_Stencil8:
                return 2;
            case ImagePixelFormat::R32G32B32_Int:
            case ImagePixelFormat::R32G32B32_UInt:
                return 3;
            case ImagePixelFormat::RGBA8:
            case ImagePixelFormat::R32G32B32A32_Int:
            case ImagePixelFormat::R32G32B32A32_UInt:
                return 4;
            default:
                COCO_ASSERT(false, "Unsupported format: %d", static_cast<int>(format));
                return 0;
        }
    }

    ImageAttachmentType ImageDescription::GetAttachmentType(ImagePixelFormat format) noexcept
    {
        switch (format)
        {
            case ImagePixelFormat::Depth24_Stencil8:
            case ImagePixelFormat::Depth32_Stencil8:
                return ImageAttachmentType::DepthStencil;
            default:
                return ImageAttachmentType::Color;
        }
    }

    ImagePixelType ImageDescription::GetPixelType(ImagePixelFormat format) noexcept
    {
        switch (format)
        {
            case ImagePixelFormat::RGBA8:
            case ImagePixelFormat::BGRA8:
                return ImagePixelType::Float;
            case ImagePixelFormat::R32_Int:
            case ImagePixelFormat::R32G32_Int:
            case ImagePixelFormat::R32G32B32_Int:
            case ImagePixelFormat::R32G32B32A32_Int:
                return ImagePixelType::Int;
            case ImagePixelFormat::R32_UInt:
            case ImagePixelFormat::R32G32_UInt:
            case ImagePixelFormat::R32G32B32_UInt:
            case ImagePixelFormat::R32G32B32A32_UInt:
                return ImagePixelType::UInt;
            default:
                return ImagePixelType::Float;
        }
    }

    RenderTargetClearValue::RenderTargetClearValue() :
        FloatColor{0.0f}
    {}

    RenderTargetClearValue::RenderTargetClearValue(const Color& clearColor) :
        RenderTargetClearValue()
    {
        Color linear = clearColor.AsLinear();
        memcpy(FloatColor, linear.Raw, sizeof(float) * 4);
    }

    RenderTargetClearValue::RenderTargetClearValue(const Vector4& clearValues) :
        FloatColor{clearValues.X(), clearValues.Y(), clearValues.Z(), clearValues.W()}
    {}

    RenderTargetClearValue::RenderTargetClearValue(const Vector4i& clearValues) :
        IntColor{clearValues.X(), clearValues.Y(), clearValues.Z(), clearValues.W()}
    {}

    RenderTargetClearValue::RenderTargetClearValue(float depth, uint8 stencil) :
        DepthStencil{depth, stencil}
    {}
}
