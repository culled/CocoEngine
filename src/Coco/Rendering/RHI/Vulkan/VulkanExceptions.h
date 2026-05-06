//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANEXCEPTIONS_H
#define COCOENGINE_VULKANEXCEPTIONS_H
#include "Coco/Core/Types/Exception.h"

namespace Coco
{
    class VulkanOperationException : public Exception
    {
    public:
        VulkanOperationException(uint32 result);
    };
} // Coco

#endif //COCOENGINE_VULKANEXCEPTIONS_H