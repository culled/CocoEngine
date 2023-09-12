#include "Renderpch.h"
#include "AttachmentFormat.h"

namespace Coco::Rendering
{
    AttachmentFormat::AttachmentFormat() :
        PixelFormat(ImagePixelFormat::Unknown),
        ShouldPreserve(false)
    {}

    AttachmentFormat::AttachmentFormat(ImagePixelFormat pixelFormat, bool shouldPreserve) :
        PixelFormat(pixelFormat),
        ShouldPreserve(shouldPreserve)
    {}

    bool AttachmentFormat::IsCompatible(const AttachmentFormat& other) const
    {
        return AreCompatible(PixelFormat, other.PixelFormat);
    }
}