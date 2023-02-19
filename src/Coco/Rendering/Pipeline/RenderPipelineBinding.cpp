#include "RenderPipelineBinding.h"

namespace Coco::Rendering
{
	RenderPipelineBinding::RenderPipelineBinding(Ref<IRenderPass> renderPass, const List<int>& passToPipelineAttachmentMapping)
	{
		List<AttachmentDescription> passAttachments = renderPass->GetAttachmentDescriptions();

		if (passAttachments.Count() != passToPipelineAttachmentMapping.Count())
			throw Exception("Attachment mappings count must match the number of attachment descriptions for the render pass");

		for (int i = 0; i < passAttachments.Count(); i++)
		{
			int targetIndex = passToPipelineAttachmentMapping[i];

			if (_mappedAttachmentDescriptions.Count() <= targetIndex)
			{
				for (int ii = _mappedAttachmentDescriptions.Count(); ii <= targetIndex; ii++)
				{
					MappedAttachmentDescription desc;
					desc.PipelineAttachmentIndex = ii;
					_mappedAttachmentDescriptions.Add(desc);
				}
			}

			if (!_mappedAttachmentDescriptions[targetIndex].AttachmentDescription.has_value())
			{
				_mappedAttachmentDescriptions[targetIndex].AttachmentDescription = passAttachments[i];
			}
			else if (_mappedAttachmentDescriptions[targetIndex].AttachmentDescription.value() != passAttachments[i])
			{
				throw Exception("Invalid remapping for render pass attachments");
			}
		}
	}

	RenderPipelineBinding::~RenderPipelineBinding()
	{
		_renderPass.reset();
		_mappedAttachmentDescriptions.Clear();
	}
}