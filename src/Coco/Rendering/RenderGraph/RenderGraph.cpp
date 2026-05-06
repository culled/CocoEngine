//
// Created by cullen on 3/8/26.
//

#include "RenderGraph.h"

#include "../Graphics/Resources/GraphicsSurface.h"

#include "Coco/Core/Engine.h"
#include "Coco/Core/Math/Math.h"
#include "Coco/Rendering/Graphics/GraphicsPlatform.h"
#include "Coco/Rendering/Graphics/GraphicsResourceCache.h"

namespace Coco
{
    RenderGraph::RenderGraph(GraphicsPlatform* platform, Allocator& allocator, const Sizei& attachmentSize) :
        _platform(platform),
        _attachmentSize(attachmentSize),
        _builder(*this),
        _nodes(&allocator, 32),
        _nextResourceID(1),
        _textureResources(&allocator),
        _attachments(),
        _currentPassAttachments(),
        _transientResources(&allocator, 16)
    {}

    void RenderGraph::AddColorAttachment(ImageColorSpace colorSpace)
    {
        ImageDescription desc = ImageDescription::Create2D(_attachmentSize.Width, _attachmentSize.Height, ImagePixelFormat::Unknown, colorSpace, ImageUsageFlags::RenderTarget, false);
        uint32 id = _nextResourceID++;
        auto& texture = _textureResources.Emplace(id, id, desc);
        texture.IsExternal = true;
        texture.IsAttachment = true;

        _attachments.EmplaceBack(id);
    }

    RenderGraphResourceRef RenderGraph::GetAttachmentResource(uint64 index) const
    {
        COCO_ASSERT(index < _attachments.GetCount(), "Invalid attachment index");
        auto& texture = _textureResources.Get(_attachments[index].TextureID);
        return texture.AsResourceRef(ResourceAccessType::RenderTarget);
    }

    const RenderGraphTextureResource& RenderGraph::GetAttachmentTextureResource(uint64 index) const
    {
        COCO_ASSERT(index < _attachments.GetCount(), "Invalid attachment index");
        return _textureResources.Get(_attachments[index].TextureID);
    }

    void RenderGraph::LinkAttachment(uint64 index, Ref<Image> attachmentImage)
    {
        COCO_ASSERT(index < _attachments.GetCount(), "Invalid attachment index");
        auto& textureRes = _textureResources.Get(_attachments[index].TextureID);
        const auto& attachmentDesc = attachmentImage->GetDescription();

        COCO_ASSERT(attachmentDesc.Width == textureRes.Desc.Width, "Attachment width does not match");
        COCO_ASSERT(attachmentDesc.Height == textureRes.Desc.Height, "Attachment height does not match");
        COCO_ASSERT(attachmentDesc.ColorSpace == textureRes.Desc.ColorSpace, "Attachment color space does not match");
        COCO_ASSERT((attachmentDesc.UsageFlags & ImageUsageFlags::RenderTarget) == ImageUsageFlags::RenderTarget, "Attachment is not a render target");

        textureRes.Desc.PixelFormat = attachmentDesc.PixelFormat;
        textureRes.TextureImage = attachmentImage;
        textureRes.Desc.UsageFlags |= attachmentDesc.UsageFlags;
    }

    void RenderGraph::GetPassAttachments(uint64 passIndex, Array<std::pair<uint64, RenderGraphAttachment>>& outAttachments) const
    {
        auto& node = _nodes.At(passIndex);
        for (const auto& output : node.Outputs)
        {
            if (output.AccessType != ResourceAccessType::RenderTarget)
                continue;

            int64 attachmentIndex = _attachments.Find(
                [id = output.ID](const RenderGraphAttachment& attachment)
            {
                return id == attachment.TextureID;
            });

            COCO_ASSERT(attachmentIndex != -1, "Attachment doesn't exist");

            outAttachments.EmplaceBack(attachmentIndex, _attachments[attachmentIndex]);
        }
    }

    uint64 RenderGraph::GetCurrentPassAttachmentHash() const
    {
        uint64 hash = 0;
        for (const auto& attachment : _currentPassAttachments)
        {
            hash = Math::CombineHashes(hash, static_cast<uint64>(attachment.Type));
        }

        return hash;
    }

    bool RenderGraph::Compile()
    {
        CullSetupNodes();

        for (uint32 i = 0; i < _nodes.GetCount(); ++i)
        {
            auto& node = _nodes[i];
            node.PassIndex = i;

            for (const auto& inputRes : node.Inputs)
            {
                AddResource(inputRes, i);
            }

            for (const auto& outputRes : node.Outputs)
            {
                AddResource(outputRes, i);
            }
        }

        return true;
    }

    void RenderGraph::Execute(const RenderScene& scene, RenderContext& ctx)
    {
        for (uint64 passIndex = 0; passIndex < _nodes.GetCount(); ++passIndex)
        {
            const auto& node = _nodes[passIndex];

            _currentPassAttachments.Clear();

            for (const auto& output : node.Outputs)
            {
                auto& outputTexture = _textureResources.Get(output.ID);

                if (!outputTexture.IsExternal && outputTexture.FirstPassIndex == node.PassIndex)
                {
                    outputTexture.TextureImage = _platform->GetResourceCache()->GetOrCreateImage(outputTexture.Desc);
                    _transientResources.Append(outputTexture.TextureImage->GetID());
                }

                Ref<Image> outputImage = outputTexture.TextureImage;
                COCO_ASSERT(outputImage, "Output image isn't valid");

                const auto& outputImageDesc = outputImage->GetDescription();

                switch (output.AccessType)
                {
                    case ResourceAccessType::RenderTarget:
                    {
                        int64 attachmentIndex = _attachments.Find([id = outputTexture.ID](const RenderGraphAttachment& attachment) { return id == attachment.TextureID; });
                        COCO_ASSERT(attachmentIndex != -1, "Attachment is invalid");

                        const RenderGraphAttachment& graphAttachment = _attachments[attachmentIndex];

                        RenderPassAttachmentInfo& attachmentInfo = _currentPassAttachments.EmplaceBack();
                        attachmentInfo.AttachmentImage = outputImage;
                        attachmentInfo.SaveResult = outputTexture.IsExternal || passIndex < outputTexture.LastPassIndex;
                        attachmentInfo.Type = outputImageDesc.AttachmentType;

                        if (outputTexture.FirstPassIndex == passIndex && graphAttachment.ClearValue.has_value())
                            attachmentInfo.ClearValue = graphAttachment.ClearValue;

                        break;
                    }
                    default:
                        // TODO
                        COCO_ASSERT(false, "Writing to textures isn't allowed yet");
                        break;
                }
            }

            ctx.BeginPass(passIndex, _currentPassAttachments);

            node.CallbackFunction(scene, ctx);

            ctx.EndPass();
        }
    }

    void RenderGraph::CullSetupNodes()
    {
        Array<uint32> unusedResourceStack(Engine::Get()->GetTemporaryStackAllocator(), _textureResources.GetCount());
        GetUnusedResources(unusedResourceStack);

        while (!unusedResourceStack.IsEmpty())
        {
            uint32& id = unusedResourceStack.Back();
            unusedResourceStack.RemoveAt(unusedResourceStack.GetCount() - 1);

            // Figure out which nodes write to this resource
            for (uint64 i = 0; i < _nodes.GetCount(); ++i)
            {
                auto& node = _nodes[i];
                int64 outputIndex = node.Outputs.Find([id](const RenderGraphResourceRef& ref) { return ref.ID == id; });
                if (outputIndex != -1)
                {
                    node.Outputs.RemoveAt(outputIndex);
                    // Node will be culled, so add its reads as unused resources
                    if (node.Outputs.IsEmpty())
                    {
                        for (const auto& inputRes : node.Inputs)
                        {
                            unusedResourceStack.Append(inputRes.ID);

                            auto resource = TryGetResource(inputRes.ID, inputRes.Type);
                            COCO_ASSERT(resource, "Resource does not exist");
                            resource->ReadCount--;
                        }
                    }
                }
            }
        }

        for (int64 i = static_cast<int64>(_nodes.GetCount()) - 1; i >= 0; --i)
        {
            if (_nodes[i].Outputs.IsEmpty())
                _nodes.RemoveAt(i);
        }
    }

    void RenderGraph::AddResource(const RenderGraphResourceRef& resourceRef, uint32 passIndex)
    {
        RenderGraphResource* resource = TryGetResource(resourceRef.ID, resourceRef.Type);
        COCO_ASSERT(resource, "Resource doesn't exist");

        resource->FirstPassIndex = Math::Min(resource->FirstPassIndex, passIndex);
        resource->LastPassIndex = Math::Max(resource->LastPassIndex, passIndex);
    }

    RenderGraphResource* RenderGraph::TryGetResource(uint32 id, ResourceType type)
    {
        switch (type)
        {
            case ResourceType::Texture:
                return _textureResources.TryGetValue(id);
            default:
                return nullptr;
        }
    }

    void RenderGraph::GetUnusedResources(Array<uint32>& unusedResources) const
    {
        for (const auto& pair : _textureResources)
        {
            if (pair.second.ReadCount > 0 || pair.second.IsExternal)
                continue;

            unusedResources.Append(pair.first);
        }
    }
} // Coco