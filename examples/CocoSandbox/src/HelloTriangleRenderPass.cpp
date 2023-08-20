#include "HelloTriangleRenderPass.h"
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/Graphics/RenderView.h>

using namespace Coco;
using namespace Coco::Rendering;

List<AttachmentDescription> HelloTriangleRenderPass::GetAttachmentDescriptions() noexcept
{
    List<AttachmentDescription> attachments;
    attachments.Add(AttachmentDescription(PixelFormat::RGBA8, ColorSpace::sRGB, false));
    attachments.Add(AttachmentDescription(PixelFormat::Depth32_Stencil8, ColorSpace::Unknown, false));
    return attachments;
}

void HelloTriangleRenderPass::Execute(RenderContext& renderContext)
{
    const Ref<RenderView>& renderView = renderContext.GetRenderView();

    for (const ObjectRenderData& objectData : renderView->Objects)
    {
        const MaterialRenderData& materialData = renderView->Materials.at(objectData.MaterialData);
        const ShaderRenderData& shaderData = renderView->Shaders.at(materialData.ShaderID);

        if(shaderData.GroupTag.empty())
		    renderContext.Draw(objectData);
    }
}
