#pragma once

#include <Coco/Core/Defines.h>
#include "../../RenderContext.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanGraphicsFence.h"
#include "VulkanCommandBuffer.h"
#include "../../../Pipeline/CompiledRenderPipeline.h"
#include "CachedResources/VulkanFramebuffer.h"
#include "CachedResources/VulkanRenderPass.h"
#include "CachedResources/VulkanPipeline.h"
#include "VulkanRenderContextCache.h"
#include "../../RenderPassShaderTypes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief Holds Vulkan data that a VulkanRenderContext uses during actual rendering
    struct VulkanContextRenderOperation
    {
        /// @brief Types of state changes
        enum class StateChangeType
        {
            None,
            Shader,
            Uniform
        };

        /// @brief The framebuffer being rendered to
        VulkanFramebuffer& Framebuffer;

        /// @brief The render pass being used
        VulkanRenderPass& RenderPass;

        /// @brief The current viewport rectangle
        RectInt ViewportRect;

        /// @brief The current scissor rectangle
        RectInt ScissorRect;

        /// @brief Semaphores to wait on before rendering
        std::vector<Ref<VulkanGraphicsSemaphore>> WaitOnSemaphores;

        /// @brief Semaphores to signal once rendering completes
        std::vector<Ref<VulkanGraphicsSemaphore>> RenderCompletedSignalSemaphores;

        /// @brief Unique state changes since the last draw call
        std::set<StateChangeType> StateChanges;

        /// @brief The currently-bound shader ID
        std::optional<uint64> CurrentShaderID;

        /// @brief The currently bound VulkanPipeline
        std::optional<VulkanPipeline*> BoundPipeline;

        /// @brief The currently bound VkDescriptorSet
        std::optional<VkDescriptorSet> BoundInstanceDescriptors;

        VulkanContextRenderOperation(VulkanFramebuffer& framebuffer, VulkanRenderPass& renderPass);
    };

    /// @brief Vulkan implementation of a RenderContext
    class VulkanRenderContext : public RenderContext, public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    public:
        /// @brief The index of the global descriptor set
        static const uint32 sGlobalDescriptorSetIndex;

        /// @brief The index of the instance descriptor set
        static const uint32 sInstanceDescriptorSetIndex;

    private:
        ManagedRef<VulkanGraphicsSemaphore> _imageAvailableSemaphore;
        ManagedRef<VulkanGraphicsSemaphore> _renderCompletedSemaphore;
        ManagedRef<VulkanGraphicsFence> _renderCompletedFence;
        UniqueRef<VulkanRenderContextCache> _cache;
        UniqueRef<VulkanCommandBuffer> _commandBuffer;

        std::optional<VulkanContextRenderOperation> _vulkanRenderOperation;
        int _backbufferIndex;

    public:
        VulkanRenderContext(const GraphicsDeviceResourceID& id);
        ~VulkanRenderContext();

        void WaitForRenderingToComplete() final;

        Ref<GraphicsSemaphore> GetImageAvailableSemaphore() final { return _imageAvailableSemaphore; }
        Ref<GraphicsSemaphore> GetRenderCompletedSemaphore() final { return _renderCompletedSemaphore; }
        Ref<GraphicsFence> GetRenderCompletedFence() final { return _renderCompletedFence; }

        void SetViewportRect(const RectInt& viewportRect) final;
        void SetScissorRect(const RectInt& scissorRect) final;
        void AddWaitOnSemaphore(Ref<GraphicsSemaphore> semaphore) final;
        void AddRenderCompletedSignalSemaphore(Ref<GraphicsSemaphore> semaphore) final;
        void SetShader(const RenderPassShaderData& shader) final;
        void Draw(const MeshData& mesh) final;

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
        void UniformChanged(UniformScope scope, ShaderUniformData::UniformKey key) final;

    private:
        /// @brief Adds image transitions before the render pass begins
        void AddPreRenderPassImageTransitions();

        /// @brief Adds image transitions after the render pass ends
        void AddPostRenderPassImageTransitions();

        /// @brief Flushes pending state changes to complete setup for a drawing operation
        /// @return True if the state was setup successfully
        bool FlushStateChanges();
    };
}