//
// Created by cullen on 3/7/26.
//

#include "RenderGraphBuilder.h"

#include "RenderGraph.h"

namespace Coco
{
    RenderGraphBuilder::RenderGraphBuilder(RenderGraph& graph) :
        _graph(&graph),
        _currentNode()
    {}

    void RenderGraphBuilder::BeginNode(const char* passName)
    {
        _currentNode.emplace(passName);
    }

    RenderGraphNode RenderGraphBuilder::EndNode()
    {
        COCO_ASSERT(_currentNode.has_value(), "Node was not started");
        return _currentNode.value();
    }

    RenderGraphResourceRef RenderGraphBuilder::CreateTexture(const ImageDescription& desc)
    {
        COCO_ASSERT(_currentNode.has_value(), "Node was not started");

        uint32 id = _graph->_nextResourceID++;
        RenderGraphTextureResource& texture = _graph->_textureResources.Emplace(id, id, desc);

        RenderGraphResourceRef newResource = texture.AsResourceRef(ResourceAccessType::ShaderWrite);
        _currentNode->Outputs.Append(newResource);
        return newResource;
    }

    RenderGraphResourceRef RenderGraphBuilder::ReadTexture(const RenderGraphResourceRef& resourceRef)
    {
        COCO_ASSERT(_currentNode.has_value(), "Node was not started");

        if (auto texture = _graph->_textureResources.TryGetValue(resourceRef.ID))
        {
            texture->ReadCount++;
            texture->Desc.UsageFlags |= ImageUsageFlags::Sampled;

            RenderGraphResourceRef newResourceRef = texture->AsResourceRef(ResourceAccessType::ShaderRead);
            _currentNode->Inputs.Append(newResourceRef);
            return newResourceRef;
        }

        COCO_ASSERT(false, "Resource does not exist");
    }

    RenderGraphResourceRef RenderGraphBuilder::WriteTexture(const RenderGraphResourceRef& resourceRef)
    {
        COCO_ASSERT(_currentNode.has_value(), "Node was not started");
        if (auto texture = _graph->_textureResources.TryGetValue(resourceRef.ID))
        {
            RenderGraphResourceRef newResourceRef = texture->AsResourceRef(ResourceAccessType::ShaderWrite);
            // TODO: update usage flags of texture resource
            // texture->Desc.UsageFlags |= ImageUsageFlags::Write
            _currentNode->Outputs.Append(newResourceRef);
            return newResourceRef;
        }

        COCO_ASSERT(false, "Resource does not exist");
    }

    RenderGraphResourceRef RenderGraphBuilder::WriteRenderTarget(uint64 attachmentIndex,
        Optional<RenderTargetClearValue> clearValue)
    {
        auto& attachment = _graph->_attachments.At(attachmentIndex);
        if (clearValue.has_value())
            attachment.ClearValue = clearValue.value();

        auto& texture = _graph->_textureResources.Get(attachment.TextureID);
        texture.Desc.UsageFlags |= ImageUsageFlags::RenderTarget;

        RenderGraphResourceRef newResourceRef = texture.AsResourceRef(ResourceAccessType::RenderTarget);
        _currentNode->Outputs.Append(newResourceRef);
        return newResourceRef;
    }

    RenderGraphResourceRef RenderGraphBuilder::CreateRenderTarget(ImagePixelFormat pixelFormat,
        Optional<RenderTargetClearValue> clearValue)
    {
        ImageDescription attachmentDesc(
            _graph->_attachmentSize.Width, _graph->_attachmentSize.Height,
            pixelFormat, ImageColorSpace::Linear,
            ImageUsageFlags::RenderTarget, false);
        uint32 id = _graph->_nextResourceID++;
        RenderGraphTextureResource& texture = _graph->_textureResources.Emplace(id, id, attachmentDesc);
        auto& attachment = _graph->_attachments.Append(RenderGraphAttachment(id));
        if (clearValue.has_value())
            attachment.ClearValue = clearValue.value();

        RenderGraphResourceRef newResourceRef = texture.AsResourceRef(ResourceAccessType::RenderTarget);
        _currentNode->Outputs.Append(newResourceRef);
        return newResourceRef;
    }

    RenderGraphResourceRef RenderGraphBuilder::WriteRenderTarget(const RenderGraphResourceRef& resourceRef)
    {
        COCO_ASSERT(_currentNode.has_value(), "Node was not started");
        if (auto texture = _graph->_textureResources.TryGetValue(resourceRef.ID))
        {
            texture->Desc.UsageFlags |= ImageUsageFlags::RenderTarget;

            RenderGraphResourceRef newResourceRef = texture->AsResourceRef(ResourceAccessType::RenderTarget);
            _currentNode->Outputs.Append(newResourceRef);
            return newResourceRef;
        }

        COCO_ASSERT(false, "Resource does not exist");
    }
} // Coco