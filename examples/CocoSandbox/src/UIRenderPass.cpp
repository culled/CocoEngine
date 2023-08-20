#include "UIRenderPass.h"
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/Graphics/RenderView.h>

using namespace Coco;
using namespace Coco::Rendering;

List<AttachmentDescription> UIRenderPass::GetAttachmentDescriptions() noexcept
{
    AttachmentDescription color(PixelFormat::RGBA8, ColorSpace::sRGB, false);
    color.ColorBlendOperation = Rendering::BlendOperation::Add;
    color.ColorSourceFactor = Rendering::BlendFactorMode::SourceAlpha;
    color.ColorDestinationFactor = Rendering::BlendFactorMode::OneMinusSourceAlpha;

    color.AlphaBlendOperation = Rendering::BlendOperation::Add;
    color.AlphaSourceBlendFactor = Rendering::BlendFactorMode::One;
    color.AlphaDestinationBlendFactor = Rendering::BlendFactorMode::One;

    List<AttachmentDescription> attachments;
    attachments.Add(color);
    return attachments;
}

void UIRenderPass::Execute(RenderContext& renderContext)
{
    const Ref<RenderView>& renderView = renderContext.GetRenderView();

    for (const ObjectRenderData& objectData : renderView->Objects)
    {
        const MaterialRenderData& materialData = renderView->Materials.at(objectData.MaterialData);
        const ShaderRenderData& shaderData = renderView->Shaders.at(materialData.ShaderID);

        if (shaderData.GroupTag == "UI")
            renderContext.Draw(objectData);
    }
}
