#include "RenderPipeline.h"

namespace Coco::Rendering
{
	RenderPipelineAttachmentDescription::RenderPipelineAttachmentDescription(const AttachmentDescription& description) :
		Description(description)
	{
	}

	RenderPipelineAttachmentDescription::~RenderPipelineAttachmentDescription()
	{
		_passesUsed.Clear();
	}

	void RenderPipelineAttachmentDescription::AddPassUse(int passIndex)
	{
		_passesUsed.Add(passIndex);

		if (_passesUsed.Count() == 1)
		{
			FirstUsePassIndex = passIndex;
			LastUsePassIndex = passIndex;
			IsUsedInSinglePass = true;
		}
		else
		{
			FirstUsePassIndex = std::min(FirstUsePassIndex, passIndex);
			LastUsePassIndex = std::max(LastUsePassIndex, passIndex);
			IsUsedInSinglePass = false;
		}

		IsUsedInFirstPipelinePass = FirstUsePassIndex == 0;
	}

	RenderPipeline::~RenderPipeline()
	{
		_attachmentDescriptions.Clear();
		_renderPasses.Clear();
	}

	List<AttachmentDescription> RenderPipeline::GetAttachmentDescriptions()
	{
		if (_attachmentDescriptionsDirty)
			GatherPipelineAttachmentDescriptions();

		List<AttachmentDescription> descriptions;

		for (const RenderPipelineAttachmentDescription& desc : _attachmentDescriptions)
		{
			descriptions.Add(desc.Description);
		}

		return descriptions;
	}

	List<RenderPipelineAttachmentDescription> RenderPipeline::GetPipelineAttachmentDescriptions()
	{
		if (_attachmentDescriptionsDirty)
			GatherPipelineAttachmentDescriptions();

		return _attachmentDescriptions;
	}

	RenderPipelineBinding* RenderPipeline::AddRenderPass(Ref<IRenderPass> renderPass, const List<int>& passToPipelineAttachmentBindings)
	{
		RenderPipelineBinding* binding = new RenderPipelineBinding(renderPass, passToPipelineAttachmentBindings);
		_renderPasses.Add(Managed<RenderPipelineBinding>(binding));
		_attachmentDescriptionsDirty = true;
		return binding;
	}

	bool RenderPipeline::RemoveRenderPass(RenderPipelineBinding* renderPassBinding)
	{
		auto it = std::find_if(_renderPasses.begin(), _renderPasses.end(), [renderPassBinding](const Managed<RenderPipelineBinding>& other) {
			return renderPassBinding == other.get();
			});

		if (it != _renderPasses.end())
		{
			_renderPasses.Erase(it);
			_attachmentDescriptionsDirty = true;
			return true;
		}

		return false;
	}

	void RenderPipeline::GatherPipelineAttachmentDescriptions()
	{
		_attachmentDescriptions.Clear();

		for (int rpI = 0; rpI < _renderPasses.Count(); rpI++)
		{
			const Managed<RenderPipelineBinding>& binding = _renderPasses[rpI];
			List<MappedAttachmentDescription> mappedPassAttachments = binding->GetMappedAttachmentDescriptions();

			if (_attachmentDescriptions.Count() < mappedPassAttachments.Count())
				_attachmentDescriptions.Resize(mappedPassAttachments.Count());

			for (int i = 0; i < mappedPassAttachments.Count(); i++)
			{
				const MappedAttachmentDescription& attachmentDescription = mappedPassAttachments[i];

				if (!attachmentDescription.AttachmentDescription.has_value())
					continue;

				RenderPipelineAttachmentDescription& pipelineAttachment = _attachmentDescriptions[i];

				if (pipelineAttachment.Description == AttachmentDescription::Empty)
					pipelineAttachment.Description = attachmentDescription.AttachmentDescription.value();

				if (pipelineAttachment.Description != attachmentDescription.AttachmentDescription.value())
					throw Exception("Conflicting render pipeline attachment binding");

				pipelineAttachment.AddPassUse(rpI);
			}
		}

		for (int i = 0; i < _attachmentDescriptions.Count(); i++)
		{
			if (_attachmentDescriptions[i].Description == AttachmentDescription::Empty)
				throw Exception("Pipeline attachments must be contiguous");
		}

		_attachmentDescriptionsDirty = false;
	}
}
