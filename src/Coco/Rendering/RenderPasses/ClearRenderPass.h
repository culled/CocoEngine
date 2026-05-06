//
// Created by cullen on 4/22/26.
//

#ifndef COCOENGINE_CLEARRENDERPASS_H
#define COCOENGINE_CLEARRENDERPASS_H
#include "Coco/Core/Types/Color.h"
#include "Coco/Rendering/RenderGraph/RenderGraphBuilder.h"

namespace Coco
{
    class ClearRenderPass
    {
    public:
        ClearRenderPass(uint8 attachmentIndex, const Color& clearColor);
        ClearRenderPass(uint8 attachmentIndex, float depthClear, uint8 stencilClear = 0);

        void Setup(RenderGraphBuilder& builder);
        void Render(const RenderScene& sceneData, RenderContext& ctx) const {}

        const RenderGraphResourceRef& GetOutputResource() const { return _output; }

    private:
        uint8 _attachmentIndex;
        RenderTargetClearValue _clearValue;

        RenderGraphResourceRef _output;
    };
} // Coco

#endif //COCOENGINE_CLEARRENDERPASS_H