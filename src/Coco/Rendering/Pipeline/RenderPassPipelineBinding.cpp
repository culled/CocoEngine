#include "Renderpch.h"
#include "RenderPassPipelineBinding.h"

namespace Coco::Rendering
{
	RenderPassPipelineBinding::RenderPassPipelineBinding(SharedRef<RenderPass> renderPass, const std::unordered_map<uint32, uint32>& attachmentMapping) :
		Pass(renderPass),
		PipelineToPassAttachmentMapping(attachmentMapping)
	{}
}
