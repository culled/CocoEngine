#include "OpaqueRenderPass.h"

List<AttachmentDescription> OpaqueRenderPass::GetAttachmentDescriptions() noexcept
{
    // Use a color and depth attachment
    return List<AttachmentDescription>({ 
        AttachmentDescription(PixelFormat::RGBA8, ColorSpace::sRGB, false), 
        AttachmentDescription(PixelFormat::Depth32_Stencil8, ColorSpace::Unknown, false)
        });
}

void OpaqueRenderPass::Execute(RenderContext& renderContext)
{
    for (auto& object : renderContext.GetRenderView()->Objects)
        renderContext.Draw(object);
}
