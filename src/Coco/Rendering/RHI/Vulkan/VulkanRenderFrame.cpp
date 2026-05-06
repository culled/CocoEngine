//
// Created by cullen on 3/22/26.
//

#include "VulkanGraphicsPlatform.h"
#include "Coco/Rendering/RenderScene.h"
#include "Coco/Rendering/RenderGraph/RenderGraph.h"

#include "Resources/VulkanGraphicsSurface.h"
#include "Resources/VulkanRenderContext.h"
#include "Resources/VulkanImage.h"

#include "VulkanRenderFrame.h"

#include "VulkanUtils.h"
#include "Coco/Rendering/Graphics/GraphicsResourceCache.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace Coco
{
    VulkanRenderTask::VulkanRenderTask(Ref<VulkanRenderContext> renderContext) :
        ImageAcquiredSemaphore(),
        RenderCompletedSemaphore(),
        RenderContext(renderContext) {}

    VulkanRenderFrame::VulkanRenderFrame(VulkanGraphicsPlatform* platform) :
        RenderFrame(platform->GetMeshStorage()),
        _platform(platform),
        _semaphores(nullptr, 2),
        _nextSemaphoreIndex(0),
        _renderContexts(nullptr, 2),
        _nextRenderContextIndex(0),
        _surfaces(nullptr, 1),
        _uniformStorage(platform, _uniformDataPageSize),
        _stagingBuffer(platform, *this)
    {
        _commandPools.EmplaceBack(_platform, VulkanQueue::Type::Graphics);
        _commandPools.EmplaceBack(_platform, VulkanQueue::Type::Transfer);
        _commandPools.EmplaceBack(_platform, VulkanQueue::Type::Compute);
    }

    VulkanRenderFrame::~VulkanRenderFrame()
    {
        _uniformStorage.Clear();

        _semaphores.Clear(true);

        for (auto& renderContext : _renderContexts)
            _platform->InvalidateResource(renderContext->GetID());

        _renderContexts.Clear(true);

        _surfaces.Clear(true);

        _commandPools.Clear(true);
    }

    void VulkanRenderFrame::NewFrame()
    {
        _stagingBuffer.WaitForWorkToComplete();

        // Only wait for contexts that actually rendered last frame
        for (uint64 i = 0; i < _nextRenderContextIndex; ++i)
            _renderContexts[i]->WaitForWorkToComplete();

        RenderFrame::NewFrame();
        _renderTasks.Clear();
        _nextSemaphoreIndex = 0;
        _nextRenderContextIndex = 0;
        _surfaces.Clear();

        for (auto& pool : _commandPools)
            pool.Reset();

        _uniformStorage.Clear();

        auto resourceCache = _platform->GetResourceCache();
        for (const auto& id : _transientResources)
            resourceCache->ReleaseResource(id);

        _transientResources.Clear();

        _stagingBuffer.NewFrame();
    }

    void VulkanRenderFrame::Render(RenderGraph&& graph, RenderScene&& scene, Ref<GraphicsSurface> surface)
    {
        Ref<VulkanGraphicsSemaphore> imageAcquiredSemaphore = GetNextSemaphore();
        Ref<VulkanImage> swapchainImage;

        Ref<VulkanGraphicsSurface> vkSurface = surface.Downcast<VulkanGraphicsSurface>();
        COCO_ASSERT(vkSurface, "Surface was null");

        // Get the swapchain image
        if (!vkSurface->AcquireImage(imageAcquiredSemaphore, swapchainImage))
            return;

        // Link the swapchain image as the first attachment
        graph.LinkAttachment(0, swapchainImage);

        VkCommandBuffer commandBuffer = AllocateCommandBuffer(VulkanQueue::Type::Graphics);

        // Start rendering
        Ref<VulkanRenderContext> renderContext = GetNextRenderContext();
        renderContext->Begin(*this, graph, scene, commandBuffer);

        graph.Execute(scene, *renderContext);

        // Transition the swapchain image for presentation
        swapchainImage->TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VulkanRenderTask& renderTask = _renderTasks.EmplaceBack(renderContext);
        renderTask.ImageAcquiredSemaphore = imageAcquiredSemaphore;
        renderTask.RenderCompletedSemaphore = vkSurface->GetRenderingCompleteSemaphore();

        _surfaces.Append(vkSurface);
        _transientResources.AppendRange(graph.GetTransientResources());
    }

    void VulkanRenderFrame::EndFrame()
    {
        bool hadSubmissions = _stagingBuffer.EndAndSubmit();

        for (auto& task : _renderTasks)
        {
            StackArray<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags2>, 2> waitSemaphores = {
                {task.ImageAcquiredSemaphore, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT}
            };

            if (hadSubmissions)
                waitSemaphores.EmplaceBack(_stagingBuffer.GetOperationsCompletedSemaphore(), VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT);

            task.RenderContext->Submit(waitSemaphores, task.RenderCompletedSemaphore);
        }

        for (auto& surface : _surfaces)
            surface->Present();
    }

    VkCommandBuffer VulkanRenderFrame::AllocateCommandBuffer(VulkanQueue::Type queueType)
    {
        return _commandPools[static_cast<uint8>(queueType)].AllocateCommandBuffer();
    }

    Matrix4x4 VulkanRenderFrame::CreateOrthographicProjection(float left, float right, float bottom, float top,
                                                              float nearClip, float farClip) const
    {
        Matrix4x4 ortho;

        float w = 2.0f / (right - left);
        float h = 2.0f / (top - bottom);
        float a = 1.0f / (nearClip - farClip);

        // This creates an orthographic matrix to transform Coco view coordinates to Vulkan with the directions: right = X, up = -Y, forward = Z
        ortho.M11() = w;
        ortho.M22() = -h;
        ortho.M33() = a;

        ortho.M14() = -(right + left) / (right - left);
        ortho.M24() = (top + bottom) / (top - bottom);
        ortho.M34() = nearClip * a;

        ortho.M44() = 1.0f;

        return ortho;
    }

    Matrix4x4 VulkanRenderFrame::CreatePerspectiveProjection(float verticalFOV, float aspectRatio, float nearClip,
        float farClip) const
    {
        Matrix4x4 perspective;

        float h = 1.0f / std::tan(verticalFOV * 0.5f);
        float w = h / aspectRatio;
        float a = -farClip / (farClip - nearClip);
        float b = nearClip * a;

        // This creates a projection matrix to transform Coco view coordinates to Vulkan with the directions: right = X, up = -Y, forward = Z
        perspective.M11() = w;
        perspective.M22() = -h;
        perspective.M33() = a;
        perspective.M44() = 0.0f;

        perspective.M34() = b;
        perspective.M43() = -1.0f;

        return perspective;
    }

    Ref<VulkanGraphicsSemaphore> VulkanRenderFrame::GetNextSemaphore()
    {
        if (_nextSemaphoreIndex >= _semaphores.GetCount())
            _semaphores.EmplaceBack(CreateDefaultManagedRef<VulkanGraphicsSemaphore>(_semaphores.GetCount(), _platform, false));

        COCO_ASSERT(_nextSemaphoreIndex < _semaphores.GetCount(), "Not enough semaphores");
        return _semaphores[_nextSemaphoreIndex++];
    }

    Ref<VulkanRenderContext> VulkanRenderFrame::GetNextRenderContext()
    {
        if (_nextRenderContextIndex >= _renderContexts.GetCount())
            _renderContexts.EmplaceBack(_platform->CreateRenderContext().Downcast<VulkanRenderContext>());

        COCO_ASSERT(_nextRenderContextIndex < _renderContexts.GetCount(), "Not enough render contexts");
        return _renderContexts[_nextRenderContextIndex++];
    }
} // Coco