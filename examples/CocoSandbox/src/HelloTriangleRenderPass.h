#pragma once

#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>

class HelloTriangleRenderPass : public Coco::Rendering::IRenderPass 
{
	virtual Coco::List<Coco::Rendering::AttachmentDescription> GetAttachmentDescriptions() override;
};