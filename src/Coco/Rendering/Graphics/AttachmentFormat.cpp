#include "Renderpch.h"
#include "AttachmentFormat.h"

namespace Coco::Rendering
{
    AttachmentFormat::AttachmentFormat() : 
        AttachmentFormat(ImagePixelFormat::Unknown, ImageColorSpace::Unknown)
    {}

    AttachmentFormat::AttachmentFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace) :
        PixelFormat(pixelFormat),
        ColorSpace(colorSpace)
    {}

    bool AttachmentFormat::IsCompatible(const AttachmentFormat& other) const
    {
        return AreCompatible(PixelFormat, other.PixelFormat) &&
            ColorSpace == other.ColorSpace;
    }
}