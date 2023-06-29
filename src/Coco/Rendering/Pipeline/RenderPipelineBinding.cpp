#include "RenderPipelineBinding.h"

namespace Coco::Rendering
{
	RenderPipelineBinding::RenderPipelineBinding(SharedRef<IRenderPass> renderPass, const List<int>& passToPipelineAttachmentMapping) :
		_renderPass(renderPass)
	{
		List<AttachmentDescription> passAttachments = renderPass->GetAttachmentDescriptions();

		if (passAttachments.Count() != passToPipelineAttachmentMapping.Count())
		{
			throw InvalidOperationException(FormattedString(
				"Attachment mappings count must match the number of attachment descriptions for the render pass. Expected {}, got {}",
				passAttachments.Count(),
				passToPipelineAttachmentMapping.Count()
			));
		}

		_mappedAttachmentDescriptions.Resize(passAttachments.Count());

		for (int i = 0; i < passAttachments.Count(); i++)
		{
			_mappedAttachmentDescriptions[i].PipelineAttachmentIndex = passToPipelineAttachmentMapping[i];
			_mappedAttachmentDescriptions[i].AttachmentDescription = passAttachments[i];
		}
	}

	RenderPipelineBinding::~RenderPipelineBinding()
	{
		_mappedAttachmentDescriptions.Clear();
	}
}