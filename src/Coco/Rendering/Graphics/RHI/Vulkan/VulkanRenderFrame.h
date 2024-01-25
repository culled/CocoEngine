#pragma once
#include "../../RenderFrame.h"
#include "VulkanMeshCache.h"
#include "VulkanUniformCache.h"
#include "VulkanRenderContext.h"
#include "VulkanCommandBuffer.h"

namespace Coco::Rendering
{
    class RenderView;
}

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanQueue;
    class VulkanGraphicsFence;
    class VulkanGraphicsSemaphore;
    class VulkanPresenter;
    class VulkanImage;

    struct VulkanRenderFrameRender
    {
        Ref<VulkanRenderContext> Context;
        SharedRef<RenderView> View;
        std::vector<Ref<VulkanGraphicsSemaphore>> WaitSemaphores;
        std::vector<Ref<VulkanGraphicsSemaphore>> SignalSemaphores;

        VulkanRenderFrameRender(Ref<VulkanRenderContext> context, SharedRef<RenderView> view);
    };

    class VulkanRenderFrame :
        public RenderFrame
    {
    public:
        VulkanRenderFrame(VulkanGraphicsDevice& device);
        ~VulkanRenderFrame();

        // Inherited via RenderFrame
        void WaitForRenderToComplete() override;
        bool IsRenderComplete() const override;
        bool Render(
            uint64 rendererID, 
            Ref<Presenter> presenter, 
            const CompiledRenderPipeline& pipeline, 
            RenderViewDataProvider& renderViewDataProvider, 
            std::span<SceneDataProvider*> sceneDataProviders) override;
        bool Render(
            uint64 rendererID,
            std::span<Ref<Image>> framebuffers,
            const SizeInt& framebufferSize,
            const CompiledRenderPipeline& pipeline,
            RenderViewDataProvider& renderViewDataProvider,
            std::span<SceneDataProvider*> sceneDataProviders) override;
        void UploadMesh(const SharedRef<Mesh>& mesh) override;
        bool TryGetCachedMesh(const Mesh& mesh, const CachedVulkanMesh*& outCachedMesh) const;

        VulkanUniformCache& GetUniformCache() { return *_uniformCache; }

        void Reset();

        void SubmitRenderTasks();

    private:
        VulkanGraphicsDevice& _device;
        VulkanQueue* _graphicsQueue;

        UniqueRef<VulkanMeshCache> _meshCache;
        UniqueRef<VulkanUniformCache> _uniformCache;

        std::vector<ManagedRef<VulkanGraphicsSemaphore>> _unusedRenderSemaphores;
        std::vector<ManagedRef<VulkanGraphicsSemaphore>> _renderSemaphores;

        std::vector<ManagedRef<VulkanGraphicsFence>> _unusedRenderFences;
        std::vector<ManagedRef<VulkanGraphicsFence>> _renderFences;

        std::vector<Ref<VulkanPresenter>> _presenters;
        std::vector<VulkanRenderFrameRender> _renders;
        std::unordered_map<GraphicsResourceID, uint64> _lastPresenterRenderIndices;
        UniqueRef<VulkanCommandBuffer> _presentCommandBuffer;

    private:
        bool AddPresenter(Ref<VulkanPresenter> presenter, Ref<VulkanGraphicsSemaphore>& outImageAcquiredSemaphore, Ref<VulkanImage>& outImage);
        Ref<VulkanRenderContext> GetRenderContext();
        Ref<VulkanGraphicsSemaphore> GetSemaphore();
        Ref<VulkanGraphicsFence> GetFence();

        void ExecuteRender(const CompiledRenderPipeline& renderPipeline, VulkanRenderFrameRender& render);
        void PresentImages();

        void DestroyFrameObjects();
    };
}