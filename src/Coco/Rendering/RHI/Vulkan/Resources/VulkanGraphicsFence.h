//
// Created by cullen on 3/24/26.
//

#ifndef COCOENGINE_VULKANGRAPHICSFENCE_H
#define COCOENGINE_VULKANGRAPHICSFENCE_H

#include "Coco/Rendering/Graphics/GraphicsResource.h"
#include "../VulkanForwardDeclarations.h"

namespace Coco
{
    class VulkanGraphicsPlatform;

    class VulkanGraphicsFence : public GraphicsResource
    {
    public:
        VulkanGraphicsFence(uint64 id, VulkanGraphicsPlatform* platform, bool startSignalled);
        ~VulkanGraphicsFence() override;

        bool IsSignalled() const;
        void WaitForSignal(bool reset);
        void Reset();

        VkFence GetFence() const { return _fence; }

    private:
        VulkanGraphicsPlatform* _platform;
        VkFence _fence;
    };
} // Coco

#endif //COCOENGINE_VULKANGRAPHICSFENCE_H