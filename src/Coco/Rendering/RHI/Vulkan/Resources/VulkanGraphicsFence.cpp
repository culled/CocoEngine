//
// Created by cullen on 3/24/26.
//

#include "VulkanGraphicsFence.h"

#include "Coco/Core/Engine.h"
#include "../VulkanGraphicsPlatform.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanUtils.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace Coco
{
    VulkanGraphicsFence::VulkanGraphicsFence(uint64 id, VulkanGraphicsPlatform* platform, bool startSignalled) :
        GraphicsResource(id),
        _platform(platform),
        _fence(nullptr)
    {
        VkFenceCreateInfo createInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        createInfo.flags = 0;

        if (startSignalled)
            createInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;

        AssertVkSuccess(vkCreateFence(_platform->GetDevice(), &createInfo, _platform->GetAllocationCallbacks(), &_fence));

        COCO_ENGINE_LOG_VERBOSE("Created VulkanGraphicsFence %u", GetID());
    }

    VulkanGraphicsFence::~VulkanGraphicsFence()
    {
        if (_fence)
        {
            _platform->WaitForIdle();
            vkDestroyFence(_platform->GetDevice(), _fence, _platform->GetAllocationCallbacks());
            _fence = nullptr;
        }

        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanGraphicsFence %u", GetID());
    }

    bool VulkanGraphicsFence::IsSignalled() const
    {
        return vkGetFenceStatus(_platform->GetDevice(), _fence) == VK_SUCCESS;
    }

    void VulkanGraphicsFence::WaitForSignal(bool reset)
    {
		vkWaitForFences(_platform->GetDevice(), 1, &_fence, VK_TRUE, std::numeric_limits<uint64>::max());

        if (reset)
            Reset();
    }

    void VulkanGraphicsFence::Reset()
    {
        AssertVkSuccess(vkResetFences(_platform->GetDevice(), 1, &_fence));
    }
} // Coco