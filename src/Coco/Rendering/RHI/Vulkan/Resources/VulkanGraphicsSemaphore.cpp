//
// Created by cullen on 3/24/26.
//

#include "VulkanGraphicsSemaphore.h"

#include "Coco/Core/Engine.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanUtils.h"
#include "../VulkanIncludes.h"

namespace Coco
{
    VulkanGraphicsSemaphore::VulkanGraphicsSemaphore(uint64 id, VulkanGraphicsPlatform* platform, bool isTimeline) :
        GraphicsResource(id),
        _platform(platform),
        _semaphore(nullptr),
        _isTimeline(isTimeline)
    {
        VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo { VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO};
        semaphoreTypeCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        semaphoreTypeCreateInfo.initialValue = 0;

        VkSemaphoreCreateInfo createInfo {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        createInfo.pNext = isTimeline ? &semaphoreTypeCreateInfo : nullptr;

        AssertVkSuccess(vkCreateSemaphore(_platform->GetDevice(), &createInfo, _platform->GetAllocationCallbacks(), &_semaphore));

        COCO_ENGINE_LOG_VERBOSE("Created VulkanGraphicsSemaphore %u", id);
    }

    VulkanGraphicsSemaphore::~VulkanGraphicsSemaphore()
    {
        if (_semaphore)
        {
            _platform->WaitForIdle();
            vkDestroySemaphore(_platform->GetDevice(), _semaphore, _platform->GetAllocationCallbacks());
            _semaphore = nullptr;
        }

        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanGraphicsSemaphore %u", GetID());
    }
} // Coco