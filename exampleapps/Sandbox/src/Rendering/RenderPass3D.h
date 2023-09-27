#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>
#include <vector>

using namespace Coco;
using namespace Coco::Rendering;

class RenderPass3D : 
	public RenderPass
{
private:
	std::vector<AttachmentFormat> _attachments;

public:
	RenderPass3D();

	std::span<const AttachmentFormat> GetInputAttachments() const { return _attachments; }
	const char* GetName() const { return "basic"; }
	bool SupportsMSAA() const final { return true; }
	void Execute(RenderContext& context, const RenderView& renderView);
};

