#include "HelloTriangleRenderPass.h"

Coco::List<Coco::Rendering::AttachmentDescription> HelloTriangleRenderPass::GetAttachmentDescriptions()
{
    Coco::List<Coco::Rendering::AttachmentDescription> attachments;
    attachments.Add(Coco::Rendering::AttachmentDescription(Coco::Rendering::PixelFormat::BGRA8, Coco::Rendering::ColorSpace::sRGB, false));
    return attachments;
}
