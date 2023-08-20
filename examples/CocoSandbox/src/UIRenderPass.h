#pragma once

#include <Coco/Rendering/Pipeline/IRenderPass.h>

using namespace Coco;
using namespace Coco::Rendering;

class UIRenderPass final : public IRenderPass
{
public:
	UIRenderPass() = default;

	string GetSubshaderName() const noexcept final { return "main"; }
	List<AttachmentDescription> GetAttachmentDescriptions() noexcept final;
	void Execute(RenderContext& renderContext) final;
};