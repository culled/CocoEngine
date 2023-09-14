#include "BasicRenderPass.h"

std::vector<AttachmentFormat> BasicRenderPass::GetInputAttachments() const
{
    return std::vector<AttachmentFormat>({
        AttachmentFormat(ImagePixelFormat::RGBA8, false)
        });
}

void BasicRenderPass::Execute(RenderContext& context, const RenderView& renderView)
{
    // TODO
}
