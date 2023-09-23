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
#include "CachedResources/VulkanRenderContextCache.h"
#include "../../RenderPassShaderTypes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanGraphicsDeviceCache;

    /// @brief The bound global state
    struct BoundGlobalState
    {
        /// @brief The ID of the shader bound
        uint64 ShaderID;

        /// @brief The bound pipeline, if any
        VulkanPipeline* Pipeline;

        /// @brief The bound descriptor set, if any
        VkDescriptorSet DescriptorSet;

        BoundGlobalState(uint64 shaderID);
    };

    /// @brief The bound instance state
    struct BoundInstanceState
    {
        /// @brief The ID of the instance bound
        uint64 InstanceID;

        /// @brief The bound descriptor set, if any
        VkDescriptorSet DescriptorSet;

        BoundInstanceState(uint64 instanceID);
    };

    /// @brief Holds Vulkan data that a VulkanRenderContext uses during actual rendering
    struct VulkanContextRenderOperation
    {
        /// @brief Types of state changes
        enum class StateChangeType
        {
            None,
            Shader,
            Uniform,
            Instance
        };

        /// @brief The framebuffer being rendered to
        VulkanFramebuffer& Framebuffer;

        /// @brief The render pass being used
        VulkanRenderPass& RenderPass;

        /// @brief The current viewport rectangle
        RectInt ViewportRect;

        /// @brief The current scissor rectangle
        RectInt ScissorRect;

        /// @brief Unique state changes since the last draw call
        std::set<StateChangeType> StateChanges;

        /// @brief The currently bound global state
        std::optional<BoundGlobalState> GlobalState;

        /// @brief The currently bound instance state
        std::optional<BoundInstanceState> InstanceState;

        VulkanContextRenderOperation(VulkanFramebuffer& framebuffer, VulkanRenderPass& renderPass);
    };

    /// @brief Vulkan implementation of a RenderContext
    class VulkanRenderContext : public RenderContext, public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        ManagedRef<VulkanGraphicsSemaphore> _renderStartSemaphore;
        ManagedRef<VulkanGraphicsSemaphore> _renderCompletedSemaphore;
        ManagedRef<VulkanGraphicsFence> _renderCompletedFence;
        UniqueRef<VulkanCommandBuffer> _commandBuffer;
        VulkanGraphicsDeviceCache& _deviceCache;

        std::vector<Ref<VulkanGraphicsSemaphore>> _waitOnSemaphores;
        std::vector<Ref<VulkanGraphicsSemaphore>> _renderCompletedSignalSemaphores;
        std::optional<VulkanContextRenderOperation> _vulkanRenderOperation;

    public:
        VulkanRenderContext(const GraphicsDeviceResourceID& id);
        ~VulkanRenderContext();

        void WaitForRenderingToComplete() final;

        Ref<GraphicsSemaphore> GetRenderStartSemaphore() final { return _renderStartSemaphore; }
        Ref<GraphicsSemaphore> GetRenderCompletedSemaphore() final { return _renderCompletedSemaphore; }
        Ref<GraphicsFence> GetRenderCompletedFence() final { return _renderCompletedFence; }

        void AddWaitOnSemaphore(Ref<GraphicsSemaphore> semaphore) final;
        void AddRenderCompletedSignalSemaphore(Ref<GraphicsSemaphore> semaphore) final;

        void SetViewportRect(const RectInt& viewportRect) final;
        void SetScissorRect(const RectInt& scissorRect) final;
        void SetMaterial(const MaterialData& material) final;
        void SetShader(const RenderPassShaderData& shader) final;

        void Draw(const MeshData& mesh, uint32 submeshID) final;
        void DrawIndexed(const MeshData& mesh, uint32 firstIndexOffset, uint32 indexCount) final;

    protected:
        bool BeginImpl() final;
        bool BeginNextPassImpl() final;
        void EndImpl() final;
        void ResetImpl() final;
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