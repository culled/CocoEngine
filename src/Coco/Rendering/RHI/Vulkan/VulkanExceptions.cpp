//
// Created by cullen on 3/22/26.
//

#include "VulkanExceptions.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace Coco
{
    VulkanOperationException::VulkanOperationException(uint32 result) :
        Exception(FormatString("Vulkan operation failed: %u", result))
    {}
} // Coco
