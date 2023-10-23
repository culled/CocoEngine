#include "Renderpch.h"
#include "AttachmentFormat.h"

namespace Coco::Rendering
{
    AttachmentFormat::AttachmentFormat() : 
        AttachmentFormat(ImagePixelFormat::Unknown, ImageColorSpace::Unknown, AttachmentClearMode::Never)
    {}

    AttachmentFormat::AttachmentFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, AttachmentClearMode clearMode) :
        PixelFormat(pixelFormat),
        ColorSpace(colorSpace),
        ClearMode(clearMode)
    {}

    bool AttachmentFormat::IsCompatible(const AttachmentFormat& other) const
    {
        return AreCompatible(PixelFormat, other.PixelFormat);
    }

    bool AttachmentFormat::IsCompatible(const ImageDescription& description) const
    {
        return PixelFormat == description.PixelFormat &&
            ColorSpace == description.ColorSpace;
    }
}