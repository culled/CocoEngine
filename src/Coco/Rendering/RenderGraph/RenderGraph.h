//
// Created by cullen on 3/8/26.
//

#ifndef COCOENGINE_RENDERGRAPH_H
#define COCOENGINE_RENDERGRAPH_H
#include <functional>

#include "RenderGraphBuilder.h"
#include "RenderGraphStorage.h"
#include "Coco/Core/Types/Map.h"
#include "Coco/Core/Memory/Refs.h"
#include "../Graphics/Resources/RenderContext.h"
#include "Coco/Rendering/Graphics/Resources/ImageTypes.h"

namespace Coco
{
    class GraphicsPlatform;
    class RenderScene;
    class GraphicsSurface;
    class Image;

    class RenderGraph
    {
        friend class RenderGraphBuilder;

    public:
        RenderGraph(GraphicsPlatform* platform, Allocator& allocator, const Sizei& attachmentSize);

        const Sizei& GetAttachmentSize() const { return _attachmentSize; }

        template<typename PassData>
        PassData CreateRenderPass(const char* name, std::function<void(RenderGraphBuilder&, PassData&)> setupFunction, std::function<void(const PassData&, const RenderScene&, RenderContext&)> callbackFunction)
        {
            _builder.BeginNode(name);

            PassData data;
            setupFunction(_builder, data);

            RenderGraphNode& node = _nodes.EmplaceBack(_builder.EndNode());
            node.CallbackFunction = [data, callbackFunction](const RenderScene& scene, RenderContext& ctx) {callbackFunction(data, scene, ctx);};

            return data;
        }

        template<typename RenderPassType, typename ... Args>
        RenderPassType CreateRenderPassObject(const char* name, Args&& ... args)
        {
            _builder.BeginNode(name);

            RenderPassType pass(std::forward<Args>(args)...);
            pass.Setup(_builder);

            RenderGraphNode& node = _nodes.EmplaceBack(_builder.EndNode());
            node.CallbackFunction = [pass](const RenderScene& scene, RenderContext& ctx) {pass.Render(scene, ctx);};

            return pass;
        }

        void AddColorAttachment(ImageColorSpace colorSpace);
        RenderGraphResourceRef GetAttachmentResource(uint64 index) const;
        uint64 GetAttachmentCount() const { return _attachments.GetCount(); }
        const RenderGraphTextureResource& GetAttachmentTextureResource(uint64 index) const;
        void LinkAttachment(uint64 index, Ref<Image> attachmentImage);
        const RenderGraphAttachment& GetAttachment(uint64 index) const { return _attachments.At(index); }
        uint64 GetPassCount() const { return _nodes.GetCount(); }
        void GetPassAttachments(uint64 passIndex, Array<std::pair<uint64, RenderGraphAttachment>>& outAttachments) const;
        Span<const RenderPassAttachmentInfo> GetCurrentPassAttachments() const { return _currentPassAttachments; }
        uint64 GetCurrentPassAttachmentHash() const;
        Span<const uint64> GetTransientResources() const { return _transientResources; }

        bool Compile();
        void Execute(const RenderScene& scene, RenderContext& ctx);

    private:
        GraphicsPlatform* _platform;
        Sizei _attachmentSize;
        RenderGraphBuilder _builder;
        Map<uint32, RenderGraphTextureResource> _textureResources;
        Array<RenderGraphNode> _nodes;
        StackArray<RenderGraphAttachment, 16> _attachments;
        StackArray<RenderPassAttachmentInfo, 16> _currentPassAttachments;
        Array<uint64> _transientResources;
        uint32 _nextResourceID;

    private:
        void CullSetupNodes();
        void AddResource(const RenderGraphResourceRef& resourceRef, uint32 passIndex);

        RenderGraphResource* TryGetResource(uint32 id, ResourceType type);
        void GetUnusedResources(Array<uint32>& unusedResources) const;
    };
} // Coco

#endif //COCOENGINE_RENDERGRAPH_H