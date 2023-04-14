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
    attachments.Add(AttachmentDescription(PixelFormat::Depth32_Stencil8, ColorSpace::sRGB, false));
    return attachments;
}

void HelloTriangleRenderPass::Execute(RenderContext* renderContext)
{
	for (const ObjectRenderData& objectData : renderContext->GetRenderView()->Objects)
		renderContext->Draw(objectData);
}
