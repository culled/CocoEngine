#include "Renderpch.h"
#include "CompiledRenderPipeline.h"
#include "RenderPipeline.h"

namespace Coco::Rendering
{
	CompiledRenderPipeline::CompiledRenderPipeline(const ResourceID& pipelineID, const ResourceVersion& pipelineVersion) :
		PipelineID(pipelineID),
		PipelineVersion(pipelineVersion),
		RenderPasses(),
		Attachments()
	{}

	std::span<const RenderPassAttachment> CompiledRenderPipeline::GetPassAttachments(uint32 passIndex) const
	{
		const auto& pipelinePass = RenderPasses.at(passIndex);
		return pipelinePass.Pass->GetAttachments();
	}
}