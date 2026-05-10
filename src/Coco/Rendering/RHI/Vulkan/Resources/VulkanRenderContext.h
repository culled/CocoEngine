//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANRENDERCONTEXT_H
#define COCOENGINE_VULKANRENDERCONTEXT_H

#include "Coco/Core/Types/Optional.h"
#include "Coco/Rendering/Graphics/Resources/RenderContext.h"
#include "../VulkanForwardDeclarations.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanUniformStorage.h"

namespace Coco
{
    class VulkanImage;
    class VulkanPipeline;
    class VulkanShaderProgram;
    class VulkanGraphicsSemaphore;
    class VulkanGraphicsFence;
    class VulkanGraphicsPlatform;
    class VulkanRenderFrame;
    class RenderGraph;
    class RenderScene;

    struct VulkanBoundShaderInfo
    {
        Ref<VulkanShaderProgram> BoundShader;
        GraphicsPipelineState BoundPipelineState;
        VulkanPipeline* BoundPipeline;

        VulkanBoundShaderInfo(Ref<VulkanShaderProgram> shaderProgram, const GraphicsPipelineState& pipelineState, VulkanPipeline& pipeline);
    };

    struct VulkanRenderOperation
    {
        VulkanRenderFrame* Frame;
        RenderGraph* Graph;
        RenderScene* Scene;
        VkCommandBuffer CommandBuffer;
        Optional<VulkanBoundShaderInfo> BoundShaderInfo;

        VulkanRenderOperation(VulkanRenderFrame& frame, RenderGraph& graph, RenderScene& scene, VkCommandBuffer commandBuffer);
    };

    class VulkanRenderContext : public RenderContext
    {
    public:
        VulkanRenderContext(uint64 id, VulkanGraphicsPlatform* platform);
        ~VulkanRenderContext();

        Sizei GetFramebufferSize() const override;
        void BeginPass(uint64 passIndex, Span<const RenderPassAttachmentInfo> passAttachments) override;
        void EndPass() override;
        void SetViewport(const Recti& viewportRect) override;
        void SetScissor(const Recti& scissorRect) override;
        void SetShader(Shader& shader, const GraphicsPipelineState& pipelineState) override;

        bool CreateAndBindGlobalBuffer(const char* name, ShaderCursor& outCursor) override;
        void BindGlobalBuffer(const char* name) override;
        bool CreateAndBindInstanceBuffer(uint64 instanceID, const char* name, ShaderCursor& outCursor) override;
        void BindInstanceBuffer(uint64 instanceID, const char* name) override;
        void SetDrawData(const void* data, uint64 dataSize, Span<const SharedPtr<Texture>> textures) override;
        void DrawObject(const RenderObject& obj) override;

        void WaitForWorkToComplete();
        void Begin(VulkanRenderFrame& frame, RenderGraph& graph, RenderScene& scene, VkCommandBuffer commandBuffer);
        void Submit(Span<const std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags2>> waitSemaphores, Ref<VulkanGraphicsSemaphore> renderingCompleteSemaphore);

    private:
        VulkanGraphicsPlatform* _platform;
        ManagedRef<VulkanGraphicsFence> _renderCompletedFence;
        Optional<VulkanRenderOperation> _currentRenderOperation;
    };
} // Coco

#endif //COCOENGINE_VULKANRENDERCONTEXT_H