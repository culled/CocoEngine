//
// Created by cullen on 3/28/26.
//

#ifndef COCOENGINE_VULKANSTAGINGBUFFER_H
#define COCOENGINE_VULKANSTAGINGBUFFER_H
#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Rendering/Graphics/PagedLinearBuffer.h"
#include "Resources/VulkanBuffer.h"
#include "Resources/VulkanGraphicsFence.h"
#include "Resources/VulkanGraphicsSemaphore.h"

namespace Coco
{
    class VulkanRenderFrame;
    class VulkanQueue;
    class VulkanGraphicsPlatform;

    struct VulkanStagingOperation : public StagingOperation
    {
        VkCommandBuffer CommandBuffer;
        VulkanQueue* Queue;

        VulkanStagingOperation(uint64 size, VkCommandBuffer commandBuffer, VulkanQueue* queue);
        VulkanStagingOperation(VulkanStagingOperation&& other) noexcept;

        friend void swap(VulkanStagingOperation& a, VulkanStagingOperation& b) noexcept;
    };

    class VulkanStagingBuffer : public StagingBuffer
    {
    public:
        VulkanStagingBuffer(VulkanGraphicsPlatform* platform, VulkanRenderFrame& renderFrame);
        ~VulkanStagingBuffer();

        StagingOperation* CreateStagingOperation(uint64 size) override;

        VulkanQueue* GetQueue() { return _transferQueue; }
        void NewFrame();
        bool EndAndSubmit();
        Ref<VulkanGraphicsSemaphore> GetOperationsCompletedSemaphore() { return _graphicsSemaphore; }
        VkCommandBuffer GetCurrentGraphicsCommandBuffer() const { return _currentGraphicsCommandBuffer; }
        void WaitForWorkToComplete();

        void AddBufferMemoryBarrier(VkBuffer targetBuffer, uint64 offset, uint64 size);
        void AddImageLayoutTransitionBarrier(VkImage targetImage, VkImageLayout newLayout, const VkImageSubresourceRange& subresourceRange);

    private:
        static constexpr uint64 _pageSize = 1024 * 1024 * 10;

        VulkanGraphicsPlatform* _platform;
        VulkanRenderFrame* _renderFrame;
        VulkanQueue* _transferQueue;
        VulkanQueue* _graphicsQueue;
        PagedLinearBuffer<VulkanBuffer> _buffers;
        Array<VulkanStagingOperation> _stagingOperations;
        ManagedRef<VulkanGraphicsSemaphore> _transferSemaphore;
        ManagedRef<VulkanGraphicsSemaphore> _graphicsSemaphore;
        ManagedRef<VulkanGraphicsFence> _fence;
        VkCommandBuffer _currentTransferCommandBuffer;
        VkCommandBuffer _currentGraphicsCommandBuffer;
        uint64 _lastSignalValue;
    };
} // Coco

#endif //COCOENGINE_VULKANSTAGINGBUFFER_H