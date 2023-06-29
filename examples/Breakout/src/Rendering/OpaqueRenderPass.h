#pragma once

#include <Coco/Rendering/Pipeline/IRenderPass.h>

using namespace Coco;
using namespace Coco::Rendering;

class OpaqueRenderPass : public IRenderPass
{
	// Inherited via IRenderPass
	virtual string GetName() const noexcept override;
	virtual List<AttachmentDescription> GetAttachmentDescriptions() noexcept override;
	virtual void Execute(RenderContext* renderContext) override;
};