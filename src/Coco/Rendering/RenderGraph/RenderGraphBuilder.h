//
// Created by cullen on 3/7/26.
//

#ifndef COCOENGINE_RENDERGRAPHBUILDER_H
#define COCOENGINE_RENDERGRAPHBUILDER_H
#include "Coco/Core/Types/Optional.h"
#include "RenderGraphTypes.h"
#include "Coco/Rendering/Graphics/Resources/ImageTypes.h"

namespace Coco
{
    class RenderGraph;
    class RenderContext;

    class RenderGraphBuilder
    {
    public:
        RenderGraphBuilder(RenderGraph& graph);

        void BeginNode(const char* passName);
        RenderGraphNode EndNode();

        RenderGraphResourceRef CreateTexture(const ImageDescription& desc);
        RenderGraphResourceRef ReadTexture(const RenderGraphResourceRef& resourceRef);
        RenderGraphResourceRef WriteTexture(const RenderGraphResourceRef& resourceRef);
        RenderGraphResourceRef WriteRenderTarget(uint64 attachmentIndex, Optional<RenderTargetClearValue> clearValue = Optional<RenderTargetClearValue>());
        RenderGraphResourceRef CreateRenderTarget(ImagePixelFormat pixelFormat, Optional<RenderTargetClearValue> clearValue = Optional<RenderTargetClearValue>());
        RenderGraphResourceRef WriteRenderTarget(const RenderGraphResourceRef& resourceRef);

    private:
        RenderGraph* _graph;
        Optional<RenderGraphNode> _currentNode;
    };
} // Coco

#endif //COCOENGINE_RENDERGRAPHBUILDER_H