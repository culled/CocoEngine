//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANGRAPHICSPLATFORMTYPES_H
#define COCOENGINE_VULKANGRAPHICSPLATFORMTYPES_H
#include <Coco/Core/Types/CoreTypes.h>

namespace Coco
{
    enum class VulkanQueueType : uint8
    {
        Graphics,
        Transfer,
        Compute
    };
}
#endif //COCOENGINE_VULKANGRAPHICSPLATFORMTYPES_H