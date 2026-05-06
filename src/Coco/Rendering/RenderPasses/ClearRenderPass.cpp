//
// Created by cullen on 4/22/26.
//

#include "ClearRenderPass.h"

namespace Coco
{
    ClearRenderPass::ClearRenderPass(uint8 attachmentIndex, const Color& clearColor) :
        _attachmentIndex(attachmentIndex),
        _clearValue(clearColor)
    {}

    ClearRenderPass::ClearRenderPass(uint8 attachmentIndex, float depthClear, uint8 stencilClear) :
        _attachmentIndex(attachmentIndex),
        _clearValue(depthClear, stencilClear)
    {}

    void ClearRenderPass::Setup(RenderGraphBuilder& builder)
    {
        _output = builder.WriteRenderTarget(_attachmentIndex, _clearValue);
    }
} // Coco