//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANRENDERFRAME_H
#define COCOENGINE_VULKANRENDERFRAME_H

#include "VulkanCommandPool.h"
#include "VulkanStagingBuffer.h"
#include "Coco/Rendering/Graphics/RenderFrame.h"
#include "Resources/VulkanGraphicsSemaphore.h"
#include "VulkanUniformStorage.h"

namespace Coco
{
    class VulkanRenderContext;
    class VulkanGraphicsPlatform;
    class VulkanGraphicsSurface;

    struct VulkanRenderTask
    {
        Ref<VulkanGraphicsSemaphore> ImageAcquiredSemaphore;
        Ref<VulkanGraphicsSemaphore> RenderCompletedSemaphore;
        Ref<VulkanRenderContext> RenderContext;

        VulkanRenderTask(Ref<VulkanRenderContext> renderContext);
    };

    class VulkanRenderFrame : public RenderFrame
    {
    public:
        VulkanRenderFrame(VulkanGraphicsPlatform* platform);
        ~VulkanRenderFrame();

        void NewFrame() override;
        void Render(RenderGraph&& graph, RenderScene&& scene, Ref<GraphicsSurface> surface) override;
        Matrix4x4 CreateOrthographicProjection(float left, float right, float bottom, float top, float nearClip, float farClip) const override;
        Matrix4x4 CreatePerspectiveProjection(float verticalFOV, float aspectRatio, float nearClip, float farClip) const override;

        void EndFrame();
        VulkanUniformStorage& GetUniformStorage() { return _uniformStorage; }
        VulkanStagingBuffer& GetStagingBuffer() { return _stagingBuffer; }
        VkCommandBuffer AllocateCommandBuffer(VulkanQueue::Type queueType);

    private:
        static constexpr int _uniformDataPageSize = 1024 * 1024;

        VulkanGraphicsPlatform* _platform;

        StackArray<VulkanCommandPool, 3> _commandPools;
        Array<ManagedRef<VulkanGraphicsSemaphore>> _semaphores;
        uint64 _nextSemaphoreIndex;

        Array<Ref<VulkanRenderContext>> _renderContexts;
        uint64 _nextRenderContextIndex;

        Array<Ref<VulkanGraphicsSurface>> _surfaces;
        Array<uint64> _transientResources;

        VulkanUniformStorage _uniformStorage;
        VulkanStagingBuffer _stagingBuffer;

        Array<VulkanRenderTask> _renderTasks;

    private:
        Ref<VulkanGraphicsSemaphore> GetNextSemaphore();
        Ref<VulkanRenderContext> GetNextRenderContext();
    };
} // Coco

#endif //COCOENGINE_VULKANRENDERFRAME_H