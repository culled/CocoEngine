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
    const Ref<RenderView> renderView = renderContext.GetRenderView();
    for (const ObjectRenderData& object : renderView->Objects)
    {
        const ResourceID& materialID = object.MaterialDatas.Count() > 0 ? object.MaterialDatas[0] : Resource::InvalidID;

        if (materialID == Resource::InvalidID)
            continue;

        renderContext.UseMaterial(materialID);

        renderContext.SetShaderMatrix4x4(ShaderDescriptorScope::Global, "_Projection", renderView->Projection);
        renderContext.SetShaderMatrix4x4(ShaderDescriptorScope::Global, "_View", renderView->View);
        renderContext.SetShaderMatrix4x4(ShaderDescriptorScope::Draw, "_Model", object.ModelMatrix);

        renderContext.Draw(object, 0);
    }
}
