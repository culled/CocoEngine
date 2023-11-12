#pragma once

#include <Coco/Core/Defines.h>
#include "../../RenderContext.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanGraphicsFence.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanGraphicsDeviceCache;
    class VulkanFramebuffer;
    class VulkanRenderPass;
    class VulkanPipeline;
    class VulkanRenderContextCache;
    class VulkanCommandBuffer;
    struct VulkanDescriptorSetLayout;
    class VulkanShaderUniformData;
    class VulkanShader;
    class VulkanImage;

    /// @brief The bound global state
    struct BoundGlobalState
    {
        /// @brief The name of the shader to bind
        string ShaderName;

        /// @brief The bound pipeline, if any
        VulkanPipeline* Pipeline;

        /// @brief The bound shader global descriptor set, if any
        VkDescriptorSet DescriptorSet;

        BoundGlobalState(const string& shaderName);
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
            Instance,
            DrawUniform
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
    class VulkanRenderContext : 
        public RenderContext, 
        public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        ManagedRef<VulkanGraphicsSemaphore> _renderStartSemaphore;
        ManagedRef<VulkanGraphicsSemaphore> _renderCompletedSemaphore;
        ManagedRef<VulkanGraphicsFence> _renderCompletedFence;
        UniqueRef<VulkanCommandBuffer> _commandBuffer;
        VulkanGraphicsDeviceCache& _deviceCache;

        std::pair<const VulkanDescriptorSetLayout*, VkDescriptorSet> _globalDescriptorSet;

        std::vector<Ref<VulkanGraphicsSemaphore>> _waitOnSemaphores;
        std::vector<Ref<VulkanGraphicsSemaphore>> _renderCompletedSignalSemaphores;
        std::optional<VulkanContextRenderOperation> _vulkanRenderOperation;

    public:
        VulkanRenderContext(const GraphicsDeviceResourceID& id);
        ~VulkanRenderContext();

        void WaitForRenderingToComplete() final;
        bool CheckForRenderingComplete() final;

        Ref<GraphicsSemaphore> GetRenderStartSemaphore() final { return _renderStartSemaphore; }
        Ref<GraphicsSemaphore> GetRenderCompletedSemaphore() final { return _renderCompletedSemaphore; }
        Ref<GraphicsFence> GetRenderCompletedFence() final { return _renderCompletedFence; }

        void AddWaitOnSemaphore(Ref<GraphicsSemaphore> semaphore) final;
        uint64 GetWaitOnSemaphoreCount() const final { return _waitOnSemaphores.size(); }
        void AddRenderCompletedSignalSemaphore(Ref<GraphicsSemaphore> semaphore) final;

        void SetViewportRect(const RectInt& viewportRect) final;
        void SetScissorRect(const RectInt& scissorRect) final;
        void SetShader(const string& shaderName) final;
        void SetMaterial(const MaterialData& material) final;
        void ClearInstanceProperties() final;

        void DrawIndexed(const MeshData& mesh, uint64 firstIndexOffset, uint64 indexCount) final;

        void SetGlobalBufferData(ShaderUniformData::UniformKey key, uint64 offset, const void* data, uint64 dataSize) final;
        void SetShaderGlobalBufferData(ShaderUniformData::UniformKey key, uint64 offset, const void* data, uint64 dataSize) final;

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

        /// @brief Adds post-render pass transitions for an image
        /// @param currentLayout The current layout of the image after rendering
        /// @param image The image
        void AddPostRenderPassImageTransitions(VkImageLayout currentLayout, VulkanImage& image);

        /// @brief Flushes pending state changes to complete setup for a drawing operation
        /// @return True if the state was setup successfully
        bool FlushStateChanges();

        /// @brief Gets the shader for the currently bound global state
        /// @return The bound shader
        VulkanShader& GetBoundShader();

        /// @brief Gets the uniform data for a shader
        /// @param shader The shader
        /// @return The uniform data for the shader
        VulkanShaderUniformData& GetUniformDataForShader(const VulkanShader& shader);
    };
}