//
// Created by cullen on 3/22/26.
//

#include "GraphicsPlatformTypes.h"

namespace Coco
{
    GraphicsDeviceCreateParams::GraphicsDeviceCreateParams() :
        PreferredDeviceType(GraphicsDeviceType::Discrete),
        SupportPresentation(true),
        RequireComputeCapability(false)
    {}
}
