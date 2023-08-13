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
    for (const ObjectRenderData& object : renderContext.GetRenderView()->Objects)
        renderContext.Draw(object);
}
