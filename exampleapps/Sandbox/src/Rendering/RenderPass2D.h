#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>
#include <Coco/Rendering/Shader.h>
#include <vector>

using namespace Coco;
using namespace Coco::Rendering;

class RenderPass2D :
	public RenderPass
{
private:
	static SharedRef<Shader> _sShader;
	std::vector<AttachmentFormat> _attachments;

public:
	RenderPass2D();

	const char* GetName() const final { return "basic"; }
	std::span<const AttachmentFormat> GetInputAttachments() const final { return _attachments; }
	void Prepare(RenderContext& context, const RenderView& renderView) final;
	void Execute(RenderContext& context, const RenderView& renderView) final;
};

