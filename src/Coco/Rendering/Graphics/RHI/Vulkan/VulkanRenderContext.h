#pragma once

#include <Coco/Core/Defines.h>
#include "../../RenderContext.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanGraphicsFence.h"
#include "VulkanCommandBuffer.h"
#include "../../../Pipeline/CompiledRenderPipeline.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderContextCache.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief Holds Vulkan data that a VulkanRenderContext uses during actual rendering
    struct VulkanContextRenderOperation
    {
        /// @brief The framebuffer being rendered to
        VulkanFramebuffer& Framebuffer;

        /// @brief The render pass being used
        VulkanRenderPass& RenderPass;

        /// @brief Semaphores to wait on before rendering
        std::vector<VulkanGraphicsSemaphore*> WaitOnSemaphores;

        /// @brief Semaphores to signal once rendering completes
        std::vector<VulkanGraphicsSemaphore*> RenderCompletedSignalSemaphores;

        VulkanContextRenderOperation(VulkanFramebuffer& framebuffer, VulkanRenderPass& renderPass);
    };

    /// @brief Vulkan implementation of a RenderContext
    class VulkanRenderContext : public RenderContext, public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        UniqueRef<VulkanGraphicsSemaphore> _imageAvailableSemaphore;
        UniqueRef<VulkanGraphicsSemaphore> _renderCompletedSemaphore;
        UniqueRef<VulkanGraphicsFence> _renderCompletedFence;
        UniqueRef<VulkanRenderContextCache> _cache;
        UniqueRef<VulkanCommandBuffer> _commandBuffer;

        std::optional<VulkanContextRenderOperation> _vulkanRenderOperation;
        int _backbufferIndex;

        RectInt _viewportRect;
        RectInt _scissorRect;

    public:
        VulkanRenderContext();
        ~VulkanRenderContext();

        void WaitForRenderingToComplete() final;

        GraphicsSemaphore* GetImageAvailableSemaphore() final { return _imageAvailableSemaphore.get(); }
        GraphicsSemaphore* GetRenderCompletedSemaphore() final { return _renderCompletedSemaphore.get(); }

        void SetViewportRect(const RectInt& viewportRect) final;
        void SetScissorRect(const RectInt& scissorRect) final;
        void AddWaitOnSemaphore(GraphicsSemaphore& semaphore) final;
        void AddRenderCompletedSignalSemaphore(GraphicsSemaphore& semaphore) final;

        /// @brief Sets the index of the backbuffer this context is rendering to when obtained via a VulkanGraphicsPresenter
        /// @param index The backbuffer index
        void SetBackbufferIndex(uint32 index);

        /// @brief Gets the index of the backbuffer this context is rendering to
        /// @return The backbuffer index, or -1 if this context was not obtained via a VulkanGraphicsPresenter
        int GetBackbufferIndex() const { return _backbufferIndex; }

    protected:
        bool ResetImpl() final;
        bool BeginImpl() final;
        bool BeginNextPassImpl() final;
        void EndImpl() final;

    private:
        /// @brief Adds image transitions before the render pass begins
        void AddPreRenderPassImageTransitions();

        /// @brief Adds image transitions after the render pass ends
        void AddPostRenderPassImageTransitions();
    };
}