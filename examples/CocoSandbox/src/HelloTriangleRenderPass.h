#pragma once

#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Core/Types/Array.h>

using namespace Coco;
using namespace Coco::Rendering;

class HelloTriangleRenderPass final : public IRenderPass 
{
public:
	HelloTriangleRenderPass() = default;

	string GetSubshaderName() const noexcept final { return "main"; }
	List<AttachmentDescription> GetAttachmentDescriptions() noexcept final;
	void Execute(RenderContext& renderContext) final;
};