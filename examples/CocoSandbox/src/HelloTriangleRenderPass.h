#pragma once

#include <Coco/Rendering/Pipeline/IRenderPass.h>

using namespace Coco;
using namespace Coco::Rendering;

class HelloTriangleRenderPass final : public IRenderPass 
{
private:
	Color _ambientColor = Color(0.25, 0.25, 0.25, 1.0);

public:
	HelloTriangleRenderPass() = default;

	string GetSubshaderName() const noexcept final { return "main"; }
	List<AttachmentDescription> GetAttachmentDescriptions() noexcept final;
	void Execute(RenderContext& renderContext) final;
};