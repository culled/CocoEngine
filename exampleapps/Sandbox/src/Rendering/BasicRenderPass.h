#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>
#include <vector>

using namespace Coco;
using namespace Coco::Rendering;

class BasicRenderPass : public RenderPass
{
public:
	std::vector<AttachmentFormat> GetInputAttachments() const;
	string GetName() const { return "basic"; }
	void Execute(RenderContext& context, const RenderView& renderView);
};

