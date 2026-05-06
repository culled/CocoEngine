//
// Created by cullen on 3/24/26.
//

#ifndef COCOENGINE_VULKANGRAPHICSSEMAPHORE_H
#define COCOENGINE_VULKANGRAPHICSSEMAPHORE_H
#include "Coco/Rendering/Graphics/GraphicsResource.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanForwardDeclarations.h"

namespace Coco
{
    class VulkanGraphicsPlatform;

    class VulkanGraphicsSemaphore : public GraphicsResource
    {
    public:
        VulkanGraphicsSemaphore(uint64 id, VulkanGraphicsPlatform* platform, bool isTimeline);
        ~VulkanGraphicsSemaphore() override;

        VkSemaphore GetSemaphore() const { return _semaphore; }
        bool IsTimeline() const { return _isTimeline; }

    private:
        VulkanGraphicsPlatform* _platform;
        VkSemaphore _semaphore;
        bool _isTimeline;
    };
} // Coco

#endif //COCOENGINE_VULKANGRAPHICSSEMAPHORE_H