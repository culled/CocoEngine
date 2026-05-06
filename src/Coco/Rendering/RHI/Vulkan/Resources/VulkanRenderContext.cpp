//
// Created by cullen on 3/22/26.
//

#include "Coco/Core/Engine.h"
#include "VulkanRenderContext.h"
#include "Coco/Rendering/RenderGraph/RenderGraph.h"
#include "Coco/Rendering/RenderScene.h"
#include "../VulkanRenderFrame.h"
#include "../VulkanGraphicsPlatform.h"
#include "VulkanGraphicsFence.h"
#include "VulkanShaderProgram.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanUtils.h"

#include "Coco/Rendering/Shader.h"
#include "Coco/Rendering/RHI/Vulkan/CachedResources/VulkanPipeline.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanShaderBufferInterface.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace Coco
{
    VulkanBoundShaderInfo::VulkanBoundShaderInfo(Ref<VulkanShaderProgram> shaderProgram,
        const GraphicsPipelineState& pipelineState, VulkanPipeline& pipeline) :
        BoundShader(std::move(shaderProgram)),
        BoundPipelineState(pipelineState),
        BoundPipeline(&pipeline)
    {}

    VulkanRenderOperation::VulkanRenderOperation(VulkanRenderFrame& frame, RenderGraph& graph, RenderScene& scene, VkCommandBuffer commandBuffer) :
        Frame(&frame),
        Graph(&graph),
        Scene(&scene),
        CommandBuffer(commandBuffer)
    {}

    VulkanRenderContext::VulkanRenderContext(uint64 id, VulkanGraphicsPlatform* platform) :
        RenderContext(id),
        _platform(platform),
        _renderCompletedFence(CreateDefaultManagedRef<VulkanGraphicsFence>(id, platform, false))
    {
        COCO_ENGINE_LOG_VERBOSE("Created VulkanRenderContext %u", id);
    }

    VulkanRenderContext::~VulkanRenderContext()
    {
        _renderCompletedFence.Invalidate();
        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanRenderContext %u", GetID());
    }

    Sizei VulkanRenderContext::GetFramebufferSize() const
    {
        if (_currentRenderOperation)
            return _currentRenderOperation->Graph->GetAttachmentSize();

        COCO_ASSERT(false, "Context wasn't rendering");
        return Sizei();
    }

    void VulkanRenderContext::BeginPass(uint64 passIndex, Span<const RenderPassAttachmentInfo> passAttachments)
    {
        COCO_ASSERT(_currentRenderOperation, "Context was not rendering");

        COCO_ASSERT(passAttachments.size() <= 16, "Up to 16 attachments are supported per pass");
        StackArray<VkRenderingAttachmentInfo, 16> colorAttachmentInfos;
        Optional<VkRenderingAttachmentInfo> depthStencilAttachmentInfo;

        for (const auto& attachmentInfo : passAttachments)
        {
            Ref<VulkanImage> attachmentImage = attachmentInfo.AttachmentImage.Downcast<VulkanImage>();
            const auto& imageDesc = attachmentImage->GetDescription();

            VkRenderingAttachmentInfo& info = attachmentInfo.Type == ImageAttachmentType::Color ?
                colorAttachmentInfos.EmplaceBack(VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO) :
                depthStencilAttachmentInfo.emplace(VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO);

            if (attachmentInfo.ClearValue)
            {
                info.clearValue = VulkanUtils::ToVkClearValue(attachmentInfo.ClearValue.value(), attachmentInfo.Type, imageDesc.PixelType);
                info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            }
            else
            {
                info.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            }

            info.storeOp = attachmentInfo.SaveResult ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;

            info.imageView = attachmentImage->GetNativeView();
            info.imageLayout = VulkanUtils::ToVkImageLayout(attachmentInfo.Type);

            attachmentImage->TransitionLayout(_currentRenderOperation->CommandBuffer, info.imageLayout);
        }

        VkRenderingInfo renderInfo{VK_STRUCTURE_TYPE_RENDERING_INFO};
        renderInfo.pColorAttachments = colorAttachmentInfos.Data();
        renderInfo.colorAttachmentCount = static_cast<uint32>(colorAttachmentInfos.GetCount());

        renderInfo.pDepthAttachment = depthStencilAttachmentInfo.has_value() ? &depthStencilAttachmentInfo.value() : nullptr;
        renderInfo.pStencilAttachment = renderInfo.pDepthAttachment; // TODO: separate stencil attachment?
        renderInfo.layerCount = 1;

        renderInfo.renderArea.offset = { 0, 0 };
        Sizei size = GetFramebufferSize();
        renderInfo.renderArea.extent = {static_cast<uint32>(size.Width), static_cast<uint32>(size.Height)};

        vkCmdBeginRendering(_currentRenderOperation->CommandBuffer, &renderInfo);
    }

    void VulkanRenderContext::EndPass()
    {
        COCO_ASSERT(_currentRenderOperation, "Context was not rendering");

        vkCmdEndRendering(_currentRenderOperation->CommandBuffer);

        for (auto& attachment : _currentRenderOperation->Graph->GetCurrentPassAttachments())
        {
            const auto& desc = attachment.AttachmentImage->GetDescription();
            if ((desc.UsageFlags | ImageUsageFlags::Sampled) == ImageUsageFlags::Sampled)
            {
                Ref<VulkanImage> vkImage = attachment.AttachmentImage.Downcast<VulkanImage>();
                vkImage->TransitionLayout(_currentRenderOperation->CommandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        }
    }

    void VulkanRenderContext::SetViewport(const Recti& viewportRect)
    {
        COCO_ASSERT(_currentRenderOperation, "Context was not rendering");

        VkViewport viewport{};
        viewport.x = static_cast<float>(viewportRect.Offset.X());
        viewport.y = static_cast<float>(viewportRect.Offset.Y());
        viewport.width = static_cast<float>(viewportRect.Size.Width);
        viewport.height = static_cast<float>(viewportRect.Size.Height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(_currentRenderOperation->CommandBuffer, 0, 1, &viewport);
    }

    void VulkanRenderContext::SetScissor(const Recti& scissorRect)
    {
        VkRect2D scissor{};
        scissor.offset.x = static_cast<uint32_t>(scissorRect.Offset.X());
        scissor.offset.y = static_cast<uint32_t>(scissorRect.Offset.Y());
        scissor.extent.width = static_cast<uint32_t>(scissorRect.Size.Width);
        scissor.extent.height = static_cast<uint32_t>(scissorRect.Size.Height);

        vkCmdSetScissor(_currentRenderOperation->CommandBuffer, 0, 1, &scissor);
    }

    void VulkanRenderContext::SetShader(Shader& shader, const GraphicsPipelineState& pipelineState)
    {
        COCO_ASSERT(_currentRenderOperation, "Context was not rendering");

        Ref<VulkanShaderProgram> shaderProgram = shader.GetProgram().Downcast<VulkanShaderProgram>();
        if (_currentRenderOperation->BoundShaderInfo &&
            _currentRenderOperation->BoundShaderInfo->BoundShader.Get() == shaderProgram.Get() &&
            _currentRenderOperation->BoundShaderInfo->BoundPipelineState == pipelineState)
            return;

        VulkanPipeline* pipeline = _platform->GetVulkanCache()->GetOrCreatePipeline(*shaderProgram, *_currentRenderOperation->Graph, pipelineState);
        vkCmdBindPipeline(_currentRenderOperation->CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

        _currentRenderOperation->BoundShaderInfo.emplace(shaderProgram, pipelineState, *pipeline);
    }

    bool VulkanRenderContext::CreateAndBindGlobalBuffer(const char* name, ShaderCursor& outCursor)
    {
        COCO_ASSERT(_currentRenderOperation, "Context wasn't rendering");
        COCO_ASSERT(_currentRenderOperation->BoundShaderInfo, "No shader has been bound");

        uint64 id = ToHash(name);
        VulkanUniformStorage& uniformStorage = _currentRenderOperation->Frame->GetUniformStorage();
        if (uniformStorage.Has(id))
        {
            uniformStorage.Bind(id, _currentRenderOperation->CommandBuffer);
            return false;
        }

        return AllocateAndBindUniformBlock(name, id, outCursor);
    }

    void VulkanRenderContext::BindGlobalBuffer(const char* name)
    {
        COCO_ASSERT(_currentRenderOperation, "Context wasn't rendering");
        COCO_ASSERT(_currentRenderOperation->BoundShaderInfo, "No shader has been bound");

        uint64 id = ToHash(name);
        _currentRenderOperation->Frame->GetUniformStorage().Bind(id, _currentRenderOperation->CommandBuffer);
    }

    bool VulkanRenderContext::CreateAndBindInstanceBuffer(uint64 instanceID, const char* name, ShaderCursor& outCursor)
    {
        COCO_ASSERT(_currentRenderOperation, "Context wasn't rendering");
        COCO_ASSERT(_currentRenderOperation->BoundShaderInfo, "No shader has been bound");

        uint64 id = Math::CombineHashes(instanceID, ToHash(name));
        VulkanUniformStorage& uniformStorage = _currentRenderOperation->Frame->GetUniformStorage();
        if (uniformStorage.Has(id))
        {
            uniformStorage.Bind(id, _currentRenderOperation->CommandBuffer);
            return false;
        }

        return AllocateAndBindUniformBlock(name, id, outCursor);
    }

    void VulkanRenderContext::BindInstanceBuffer(uint64 instanceID, const char* name)
    {
        COCO_ASSERT(_currentRenderOperation, "Context wasn't rendering");
        COCO_ASSERT(_currentRenderOperation->BoundShaderInfo, "No shader has been bound");

        uint64 id = Math::CombineHashes(instanceID, ToHash(name));
        _currentRenderOperation->Frame->GetUniformStorage().Bind(id, _currentRenderOperation->CommandBuffer);
    }

    void VulkanRenderContext::SetDrawData(const void* data, uint64 dataSize, Span<const SharedPtr<Texture>> textures)
    {
        COCO_ASSERT(_currentRenderOperation, "Context wasn't rendering");
        COCO_ASSERT(_currentRenderOperation->BoundShaderInfo, "No shader has been bound");

        auto shader = _currentRenderOperation->BoundShaderInfo->BoundShader;
        auto pipelineLayout = shader->GetPipelineLayout();

        // Write data to the push constant buffer
        if (dataSize > 0)
        {
            for (const auto& range : pipelineLayout->PushConstantRanges)
            {
                const uint8* dataPtr = static_cast<const uint8*>(data) + range.offset;
                if (range.offset + range.size > dataSize)
                {
                    COCO_ASSERT(false, "Data size is not large enough for push constant range. Required size: %u, given size: %u", range.offset + range.size, dataSize);
                    continue;
                }
                vkCmdPushConstants(_currentRenderOperation->CommandBuffer, pipelineLayout->PipelineLayout, range.stageFlags, range.offset, range.size, dataPtr);
            }
        }

        // Write textures to a descriptor set and bind it
        if (!textures.empty())
            _currentRenderOperation->Frame->GetUniformStorage().BindDrawTextures(textures, shader, _currentRenderOperation->CommandBuffer);
    }

    void VulkanRenderContext::DrawObject(const RenderObject& obj)
    {
        COCO_ASSERT(_currentRenderOperation, "Context wasn't rendering");
        COCO_ASSERT(_currentRenderOperation->BoundShaderInfo, "No shader has been bound");

        const MeshEntry* meshEntry = _platform->GetMeshStorage()->GetMesh(obj.MeshID);

        StackArray<VkBuffer, 5> buffers;
        StackArray<VkDeviceSize, 5> bufferOffsets;
        VkBuffer buffer = meshEntry->MeshBuffer.Downcast<VulkanBuffer>()->GetBuffer();

        for (const auto& channel : _currentRenderOperation->BoundShaderInfo->BoundShader->GetVertexChannels())
        {
            uint64 offset = meshEntry->ChannelOffsets[static_cast<uint8>(channel)] + meshEntry->BufferOffset;
            bufferOffsets.Append(offset);
            buffers.Append(buffer);
        }

        vkCmdBindVertexBuffers(_currentRenderOperation->CommandBuffer, 0, static_cast<uint32>(buffers.GetCount()), buffers.Data(), bufferOffsets.Data());

        uint64 indexDataOffset = meshEntry->BufferOffset + meshEntry->IndexDataOffset;
        vkCmdBindIndexBuffer(_currentRenderOperation->CommandBuffer, buffer, indexDataOffset, VK_INDEX_TYPE_UINT32);

        // Draw the mesh
        vkCmdDrawIndexed(_currentRenderOperation->CommandBuffer,
            obj.DrawSubmesh.IndexCount,
            1,
            obj.DrawSubmesh.IndexOffset,
            obj.DrawSubmesh.VertexOffset,
            0);

        _currentRenderOperation->Frame->AddDrawCall(obj.DrawSubmesh.IndexCount / 3, obj.DrawSubmesh.IndexCount);
    }

    void VulkanRenderContext::WaitForWorkToComplete()
    {
        _renderCompletedFence->WaitForSignal(true);
    }

    void VulkanRenderContext::Begin(VulkanRenderFrame& frame, RenderGraph& graph, RenderScene& scene,
                                    VkCommandBuffer commandBuffer)
    {
        _currentRenderOperation.emplace(frame, graph, scene, commandBuffer);

        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        Recti viewportRect(Vector2i::Zero, graph.GetAttachmentSize());
        SetViewport(viewportRect);
        SetScissor(viewportRect);

        vkCmdSetLineWidth(_currentRenderOperation->CommandBuffer, 1.0f);
    }

    void VulkanRenderContext::Submit(Span<const std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags2>> waitSemaphores, Ref<VulkanGraphicsSemaphore> renderingCompleteSemaphore)
    {
        COCO_ASSERT(_currentRenderOperation, "Context was not rendering");
        vkEndCommandBuffer(_currentRenderOperation->CommandBuffer);

        uint64 currentFrameNumber = _platform->GetCurrentFrameNumber();

        COCO_ASSERT(waitSemaphores.size() <= 2, "Up to 2 waits are supported currently");
        StackArray<VkSemaphoreSubmitInfo, 2> waitInfos;
        for (const auto& pair : waitSemaphores)
        {
            VkSemaphoreSubmitInfo& info = waitInfos.EmplaceBack(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr);
            info.semaphore = pair.first->GetSemaphore();
            info.value = currentFrameNumber;
            info.stageMask = pair.second;
            info.deviceIndex = 0;
        }

        VkSemaphoreSubmitInfo signalInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            nullptr,
            renderingCompleteSemaphore->GetSemaphore(),
            0,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            0
        };

        VkCommandBufferSubmitInfo bufferInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            nullptr,
            _currentRenderOperation->CommandBuffer,
            0
        };

        VkSubmitInfo2 submitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            nullptr,
            0,
            static_cast<uint32>(waitInfos.GetCount()), waitInfos.Data(),
            1, &bufferInfo,
            1, &signalInfo
        };

        VulkanQueue* graphicsQueue = _platform->GetQueue(VulkanQueue::Type::Graphics);
        vkQueueSubmit2(graphicsQueue->GetQueue(), 1, &submitInfo, _renderCompletedFence->GetFence());
    }

    bool VulkanRenderContext::AllocateAndBindUniformBlock(const char* name, uint64 id, ShaderCursor& outCursor) {
        VulkanShaderBufferInterface* interface = _currentRenderOperation->Frame->GetUniformStorage().Allocate(id, _currentRenderOperation->BoundShaderInfo->BoundShader, name, _currentRenderOperation->CommandBuffer);
        if (!interface)
            return false;

        interface->Bind(_currentRenderOperation->CommandBuffer);
        outCursor.BindToInterface(*interface);
        return true;
    }
} // Coco