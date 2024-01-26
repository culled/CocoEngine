#pragma once
#include <Coco/Core/Types/Refs.h>
#include "../../RenderContext.h"
#include "VulkanIncludes.h"
#include "../../ShaderUniformTypes.h"

namespace Coco::Rendering
{
    struct CompiledRenderPipeline;
    class RenderView;
    class ShaderUniformLayout;
}

namespace Coco::Rendering::Vulkan
{ 
    class VulkanGraphicsDevice;
    class VulkanCommandBuffer;
    class VulkanGraphicsFence;
    class VulkanPipeline;
    class VulkanRenderPass;
    class VulkanRenderFrame;
    class VulkanDescriptorSetLayout;
    class VulkanShader;
    struct CachedVulkanUniformData;
    class VulkanGraphicsSemaphore;

    struct VulkanGlobalState
    {
        SharedRef<Shader> BoundShader;
        VulkanShader& BoundVulkanShader;
        VulkanPipeline& BoundPipeline;
        VkDescriptorSet GlobalSet;
        VkDescriptorSet GlobalShaderSet;

        VulkanGlobalState(SharedRef<Shader> shader, VulkanShader& vulkanShader, VulkanPipeline& pipeline);
    };

    struct VulkanInstanceState
    {
        uint64 InstanceID;

        VkDescriptorSet InstanceSet;

        VulkanInstanceState(uint64 instanceID);
    };

    struct VulkanRenderContextOperation
    {
        const CompiledRenderPipeline& Pipeline;
        RenderView& View;
        VulkanCommandBuffer& CommandBuffer;
        uint32 CurrentPassIndex;
        VkDescriptorSet GlobalSet;
        VulkanRenderPass& RenderPass;
        VulkanRenderFrame& RenderFrame;

        VulkanRenderContextOperation(
            const CompiledRenderPipeline& pipeline, 
            RenderView& view, 
            VulkanCommandBuffer& commandBuffer,
            VulkanRenderPass& renderPass,
            VulkanRenderFrame& renderFrame);
    };

    class VulkanRenderContext :
        public RenderContext
    {
    public:
        VulkanRenderContext(const GraphicsResourceID& id, VulkanGraphicsDevice& device);
        ~VulkanRenderContext();

        // Inherited via RenderContext
        void SetViewportRect(const RectInt& viewportRect) override;
        void SetScissorRect(const RectInt& scissorRect) override;
        void SetShader(const SharedRef<Shader>& shader) override;
        void Draw(const SharedRef<Mesh>& mesh, const Submesh& submesh) override;
        void DrawIndexed(const SharedRef<Mesh>& mesh, uint64 indexOffset, uint64 indexCount) override;

        void WaitForRenderToComplete();
        bool IsIdle() const;

        void SetGlobalUniforms(std::span<const ShaderUniformValue> uniforms) override;
        void SetGlobalShaderUniforms(std::span<const ShaderUniformValue> uniforms) override;
        void SetMaterial(const SharedRef<Material>& material) override;
        void SetInstanceUniforms(uint64 instanceID, std::span<const ShaderUniformValue> uniforms) override;
        void SetDrawUniforms(std::span<const ShaderUniformValue> uniforms) override;

        void Begin(const CompiledRenderPipeline& pipeline, RenderView& renderView, VulkanRenderFrame& renderFrame);
        void BeginNextPass();
        void End(std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>> waitSemaphores, std::span<Ref<VulkanGraphicsSemaphore>> signalSemaphores);
        void Reset();

    private:
        static const uint64 _globalDataID;

        VulkanGraphicsDevice& _device;
        UniqueRef<VulkanCommandBuffer> _commandBuffer;
        ManagedRef<VulkanGraphicsFence> _renderCompletedFence;
        RenderContextStats _renderStats;

        std::optional<VulkanRenderContextOperation> _renderOperation;
        std::optional<VulkanGlobalState> _globalState;
        std::optional<VulkanInstanceState> _instanceState;

        std::vector<ShaderUniformValue> _drawUniforms;

    private:
        void CreateCommandBuffer();
        void FreeCommandBuffer();

        void AddPreRenderPassAttachmentTransitions();
        void AddPostRenderPassAttachmentTransitions();

        VkDescriptorSet GetOrCreateDescriptorSet(uint64 dataID, UniformScope scope, VulkanDescriptorSetLayout& descriptorSetLayout);
        void BindDescriptorSet(VkDescriptorSet set, uint32 offset);

        bool FlushStateChanges();
        void FlushPushConstants(const ShaderUniformLayout& drawLayout);

        void SetUniforms(uint64 dataID, uint64 dataVersion, UniformScope scope, std::span<const ShaderUniformValue> uniforms);
        void WriteDescriptorSetData(VkDescriptorSet set, const VulkanDescriptorSetLayout& descriptorSetLayout, const CachedVulkanUniformData& cachedData);
        void WriteDescriptorSetTextures(
            VkDescriptorSet set,
            const VulkanDescriptorSetLayout& descriptorSetLayout,
            const std::unordered_map<string, WeakSharedRef<Texture>>& textures);
    };
}
