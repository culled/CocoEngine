#include "Renderpch.h"
#include "AttachmentFormat.h"

namespace Coco::Rendering
{
    AttachmentFormat::AttachmentFormat() : 
        AttachmentFormat(ImagePixelFormat::Unknown, ImageColorSpace::Unknown, AttachmentClearMode::Never, false)
    {}

    AttachmentFormat::AttachmentFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, AttachmentClearMode clearMode, bool preserveAfterRender) :
        PixelFormat(pixelFormat),
        ColorSpace(colorSpace),
        ClearMode(clearMode),
        PreserveAfterRender(preserveAfterRender)
    {}

    bool AttachmentFormat::IsCompatible(const AttachmentFormat& other) const
    {
        return AreCompatible(PixelFormat, other.PixelFormat) &&
            ColorSpace == other.ColorSpace;
    }
}