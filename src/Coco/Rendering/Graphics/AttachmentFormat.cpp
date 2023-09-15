#include "Renderpch.h"
#include "AttachmentFormat.h"

namespace Coco::Rendering
{
    AttachmentFormat::AttachmentFormat() : 
        AttachmentFormat(ImagePixelFormat::Unknown, false)
    {}

    AttachmentFormat::AttachmentFormat(ImagePixelFormat pixelFormat, bool shouldPreserve, MSAASamples msaaSamples) :
        PixelFormat(pixelFormat),
        ShouldPreserve(shouldPreserve),
        SampleCount(msaaSamples)
    {}

    bool AttachmentFormat::IsCompatible(const AttachmentFormat& other) const
    {
        return AreCompatible(PixelFormat, other.PixelFormat);
    }
}