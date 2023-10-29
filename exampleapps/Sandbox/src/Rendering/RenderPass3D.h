#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>
#include <Coco/Rendering/Graphics/BufferDataLayout.h>
#include <vector>

using namespace Coco;
using namespace Coco::Rendering;

class RenderPass3D : 
	public RenderPass
{
public:
	static const BufferDataLayout sLightingBufferLayout;

private:
	std::vector<AttachmentFormat> _attachments;

public:
	RenderPass3D();

	const char* GetName() const final { return "basic"; }
	std::span<const AttachmentFormat> GetInputAttachments() const final{ return _attachments; }
	void Prepare(RenderContext& context, const RenderView& renderView) final;
	void Execute(RenderContext& context, const RenderView& renderView) final;
};

