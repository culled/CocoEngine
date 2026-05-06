//
// Created by cullen on 3/9/26.
//

#include "Image.h"
#include <cmath>

#include "Coco/Core/Asserts.h"

namespace Coco
{
    uint64 Image::GetPixelDataSize() const
    {
        uint8 bytesPerPixel = ImageDescription::GetBytesPerPixel(_description.PixelFormat);
        uint8 channelCount = ImageDescription::GetChannelCount(_description.PixelFormat);

        return _description.Width * _description.Height * _description.Depth * bytesPerPixel * channelCount;
    }

    Image::Image(uint64 id, const ImageDescription& description) :
        GraphicsResource(id),
        _description(description)
    {}
} // Coco