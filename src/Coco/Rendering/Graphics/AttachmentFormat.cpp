#include "Renderpch.h"
#include "AttachmentFormat.h"

namespace Coco::Rendering
{
    AttachmentFormat::AttachmentFormat() : 
        AttachmentFormat(ImagePixelFormat::Unknown, ImageColorSpace::Unknown, false)
    {}

    AttachmentFormat::AttachmentFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace, bool shouldPreserve, MSAASamples msaaSamples) :
        PixelFormat(pixelFormat),
        ColorSpace(colorSpace),
        ShouldPreserve(shouldPreserve),
        SampleCount(msaaSamples)
    {}

    bool AttachmentFormat::IsCompatible(const AttachmentFormat& other) const
    {
        return AreCompatible(PixelFormat, other.PixelFormat);
    }
}