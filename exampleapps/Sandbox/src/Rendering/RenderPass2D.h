#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>
#include <vector>

using namespace Coco;
using namespace Coco::Rendering;

class RenderPass2D :
	public RenderPass
{
private:
	std::vector<AttachmentFormat> _attachments;

public:
	RenderPass2D();

	std::span<const AttachmentFormat> GetInputAttachments() const { return _attachments; }
	string GetName() const { return "basic"; }
	bool SupportsMSAA() const final { return false; }
	void Execute(RenderContext& context, const RenderView& renderView);
};

