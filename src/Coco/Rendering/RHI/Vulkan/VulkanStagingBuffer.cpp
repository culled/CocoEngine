//
// Created by cullen on 3/28/26.
//

#include "VulkanStagingBuffer.h"

#include "VulkanGraphicsPlatform.h"
#include "VulkanQueue.h"
#include "VulkanRenderFrame.h"
#include "VulkanUtils.h"

namespace Coco
{
    VulkanStagingOperation::VulkanStagingOperation(uint64 size, VkCommandBuffer commandBuffer, VulkanQueue* queue) :
        StagingOperation(size),
        CommandBuffer(commandBuffer),
        Queue(queue)
    {}

    VulkanStagingOperation::VulkanStagingOperation(VulkanStagingOperation&& other) noexcept :
        StagingOperation(),
        CommandBuffer(nullptr),
        Queue(nullptr)
    {
        swap(*this, other);
    }

    VulkanStagingBuffer::VulkanStagingBuffer(VulkanGraphicsPlatform* platform, VulkanRenderFrame& renderFrame) :
        _platform(platform),
        _renderFrame(&renderFrame),
        _transferQueue(platform->GetQueue(VulkanQueue::Type::Transfer)),
        _graphicsQueue(platform->GetQueue(VulkanQueue::Type::Graphics)),
        _buffers(platform, BufferDescription(_pageSize, BufferUsageFlags::HostVisible | BufferUsageFlags::TransferSource), platform->GetDeviceDescription().MinimumBufferAlignment),
        _stagingOperations(),
        _transferSemaphore(CreateDefaultManagedRef<VulkanGraphicsSemaphore>(0, platform, false)),
        _graphicsSemaphore(CreateDefaultManagedRef<VulkanGraphicsSemaphore>(0, platform, true)),
        _fence(CreateDefaultManagedRef<VulkanGraphicsFence>(0, platform, true)),
        _currentTransferCommandBuffer(nullptr),
        _currentGraphicsCommandBuffer(nullptr),
        _lastSignalValue(0)
    {}

    VulkanStagingBuffer::~VulkanStagingBuffer()
    {
        _fence.Invalidate();
        _graphicsSemaphore.Invalidate();
        _transferSemaphore.Invalidate();
        _stagingOperations.Clear(true);
        _buffers.Clear();
    }

    StagingOperation* VulkanStagingBuffer::CreateStagingOperation(uint64 size)
    {
        VulkanStagingOperation& operation = _stagingOperations.EmplaceBack(size, _currentTransferCommandBuffer, _transferQueue);
        Ref<VulkanBuffer> buffer;
        _buffers.Allocate(size, buffer, operation.BufferOffset);
        operation.StagingBuffer = buffer;
        operation.BufferPtr = static_cast<uint8*>(buffer->GetMappedPtr()) + operation.BufferOffset;
        return &operation;
    }

    void VulkanStagingBuffer::NewFrame()
    {
        _stagingOperations.Clear();
        _buffers.Clear();

        _currentTransferCommandBuffer = _renderFrame->AllocateCommandBuffer(VulkanQueue::Type::Transfer);

        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(_currentTransferCommandBuffer, &beginInfo);

        _currentGraphicsCommandBuffer = _renderFrame->AllocateCommandBuffer(VulkanQueue::Type::Graphics);
        vkBeginCommandBuffer(_currentGraphicsCommandBuffer, &beginInfo);
    }

    bool VulkanStagingBuffer::EndAndSubmit()
    {
        vkEndCommandBuffer(_currentTransferCommandBuffer);
        vkEndCommandBuffer(_currentGraphicsCommandBuffer);

        if (_stagingOperations.IsEmpty())
            return false;

        _fence->Reset();

        uint64 signalValue = _platform->GetCurrentFrameNumber() + 1;

        VkSemaphoreSubmitInfo transferWaitInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            nullptr,
            _graphicsSemaphore->GetSemaphore(),
            _lastSignalValue,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            0
        };

        VkCommandBufferSubmitInfo transferBufferInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            nullptr,
            _currentTransferCommandBuffer,
            0
        };

        VkSemaphoreSubmitInfo transferSignalInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            nullptr,
            _transferSemaphore->GetSemaphore(),
            signalValue,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            0
        };

        VkSubmitInfo2 transferSubmitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            nullptr,
            0,
            1, &transferWaitInfo,
            1, &transferBufferInfo,
            1, &transferSignalInfo
        };

        vkQueueSubmit2(_transferQueue->GetQueue(), 1, &transferSubmitInfo, nullptr);

        VkSemaphoreSubmitInfo graphicsWaitInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            nullptr,
            _transferSemaphore->GetSemaphore(),
            0,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            0
        };

        VkCommandBufferSubmitInfo graphicsBufferInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            nullptr,
            _currentGraphicsCommandBuffer,
            0
        };

        VkSemaphoreSubmitInfo graphicsSignalInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            nullptr,
            _graphicsSemaphore->GetSemaphore(),
            signalValue,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            0
        };

        VkSubmitInfo2 graphicsSubmitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            nullptr,
            0,
            1, &graphicsWaitInfo,
            1, &graphicsBufferInfo,
            1, &graphicsSignalInfo
        };

        vkQueueSubmit2(_graphicsQueue->GetQueue(), 1, &graphicsSubmitInfo, _fence->GetFence());

        _lastSignalValue = signalValue;

        return true;
    }

    void VulkanStagingBuffer::WaitForWorkToComplete()
    {
        _fence->WaitForSignal(false);
    }

    void VulkanStagingBuffer::AddBufferMemoryBarrier(VkBuffer targetBuffer, uint64 offset, uint64 size)
    {
        uint32 graphicsQueueIndex = _graphicsQueue->GetFamilyIndex();
        uint32 transferQueueIndex = _transferQueue->GetFamilyIndex();

        if (transferQueueIndex == graphicsQueueIndex)
        {
            VkMemoryBarrier2 memoryBarrier = {
                VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                nullptr,
                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                VK_ACCESS_2_MEMORY_WRITE_BIT,
                VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
                VK_ACCESS_2_MEMORY_READ_BIT
            };

            VkDependencyInfo dependencyInfo = {
                VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                nullptr,
                0,
                1, &memoryBarrier,
                0, nullptr,
                0, nullptr
            };

            vkCmdPipelineBarrier2(_currentTransferCommandBuffer, &dependencyInfo);
        }
        else
        {
            VkBufferMemoryBarrier2 transferMemoryBarrier = {
                VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                nullptr,
                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                VK_ACCESS_2_MEMORY_WRITE_BIT,
                VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
                transferQueueIndex, graphicsQueueIndex,
                targetBuffer,
                offset, size
            };

            VkDependencyInfo transferBarrierDependencyInfo = {
                VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                nullptr,
                0,
                0, nullptr,
                1, &transferMemoryBarrier,
                0, nullptr
            };

            vkCmdPipelineBarrier2(_currentTransferCommandBuffer, &transferBarrierDependencyInfo);

            VkBufferMemoryBarrier2 graphicsMemoryBarrier = {
                VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                nullptr,
                VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
                VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
                VK_ACCESS_2_MEMORY_READ_BIT,
                transferQueueIndex, graphicsQueueIndex,
                targetBuffer,
                offset, size
            };

            VkDependencyInfo graphicsBarrierDependencyInfo = {
                VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                nullptr,
                0,
                0, nullptr,
                1, &graphicsMemoryBarrier,
                0, nullptr
            };

            vkCmdPipelineBarrier2(_currentGraphicsCommandBuffer, &graphicsBarrierDependencyInfo);
        }
    }

    void VulkanStagingBuffer::AddImageLayoutTransitionBarrier(VkImage targetImage, VkImageLayout newLayout, const VkImageSubresourceRange& subresourceRange)
    {
        uint32 graphicsQueueIndex = _graphicsQueue->GetFamilyIndex();
        uint32 transferQueueIndex = _transferQueue->GetFamilyIndex();

        if (transferQueueIndex == graphicsQueueIndex)
        {
            VkImageMemoryBarrier2 imageMemoryBarrier = {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                nullptr,
                VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_MEMORY_WRITE_BIT,
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, newLayout,
                VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
                targetImage,
                subresourceRange
            };

            VkDependencyInfo imageBarrierDependencyInfo = {
                VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                nullptr,
                0,
                0, nullptr,
                0, nullptr,
                1, &imageMemoryBarrier
            };

            vkCmdPipelineBarrier2(_currentTransferCommandBuffer, &imageBarrierDependencyInfo);
        }
        else
        {
            VkImageMemoryBarrier2 postCopyTransferMemoryBarrier = {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                nullptr,
                VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_MEMORY_WRITE_BIT,
                VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, newLayout,
                transferQueueIndex, graphicsQueueIndex,
                targetImage,
                subresourceRange
            };

            VkDependencyInfo postCopyTransferDependencyInfo = {
                VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                nullptr,
                0,
                0, nullptr,
                0, nullptr,
                1, &postCopyTransferMemoryBarrier
            };

            vkCmdPipelineBarrier2(_currentTransferCommandBuffer, &postCopyTransferDependencyInfo);

            VkImageMemoryBarrier2 postCopyGraphicsMemoryBarrier = {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                nullptr,
                VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, newLayout,
                transferQueueIndex, graphicsQueueIndex,
                targetImage,
                subresourceRange
            };

            VkDependencyInfo postCopyGraphicsDependencyInfo = {
                VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                nullptr,
                0,
                0, nullptr,
                0, nullptr,
                1, &postCopyGraphicsMemoryBarrier
            };

            vkCmdPipelineBarrier2(_currentGraphicsCommandBuffer, &postCopyGraphicsDependencyInfo);
        }
    }

    void swap(VulkanStagingOperation& a, VulkanStagingOperation& b) noexcept
    {
        using std::swap;

        swap(a.StagingBuffer, b.StagingBuffer);
        swap(a.BufferPtr, b.BufferPtr);
        swap(a.BufferOffset, b.BufferOffset);
        swap(a.Size, b.Size);
        swap(a.CommandBuffer, b.CommandBuffer);
        swap(a.Queue, b.Queue);
    }
} // Coco