//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANQUEUE_H
#define COCOENGINE_VULKANQUEUE_H

#include "Coco/Core/Types/CoreTypes.h"
#include "VulkanForwardDeclarations.h"

namespace Coco
{
    class VulkanGraphicsPlatform;

    class VulkanQueue
    {
    public:
        enum class Type
        {
            Graphics,
            Transfer,
            Compute
        };

    public:
        VulkanQueue(VulkanGraphicsPlatform* platform, Type type, VkQueue queue, uint32 familyIndex);

        Type GetType() const { return _type; }
        uint32 GetFamilyIndex() const { return _queueFamilyIndex; }
        VkQueue GetQueue() const { return _queue; }

        void WaitForIdle();

    private:
        VulkanGraphicsPlatform* _platform;
        Type _type;
        uint32 _queueFamilyIndex;
        VkQueue _queue;
    };
} // Coco

#endif //COCOENGINE_VULKANQUEUE_H