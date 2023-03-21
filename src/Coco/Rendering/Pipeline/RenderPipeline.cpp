#include "RenderPipeline.h"

#include "../RenderingService.h"

namespace Coco::Rendering
{
	RenderPipelineAttachmentDescription::RenderPipelineAttachmentDescription(const AttachmentDescription& description) noexcept :
		Description(description)
	{}

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
			FirstUsePassIndex = Math::Min(FirstUsePassIndex, passIndex);
			LastUsePassIndex = Math::Max(LastUsePassIndex, passIndex);
			IsUsedInSinglePass = false;
		}

		IsUsedInFirstPipelinePass = FirstUsePassIndex == 0;
	}

	RenderPipeline::RenderPipeline() : RenderingResource(ResourceType::RenderPipeline),
		_clearColor(Color::Black)
	{}

	RenderPipeline::~RenderPipeline()
	{
		_attachmentDescriptions.Clear();
		_renderPasses.Clear();
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

	bool RenderPipeline::RemoveRenderPass(RenderPipelineBinding* renderPassBinding) noexcept
	{
		try
		{
			auto it = _renderPasses.Find([renderPassBinding](const Managed<RenderPipelineBinding>& other) {
				return renderPassBinding == other.get();
				});

			if (it != _renderPasses.end())
			{
				_renderPasses.Remove(it);
				_attachmentDescriptionsDirty = true;
				return true;
			}
		}
		catch(...)
		{ }

		return false;
	}

	List<RenderPipelineBinding*> RenderPipeline::GetPasses() const
	{
		List<RenderPipelineBinding*> passes;

		for (const Managed<RenderPipelineBinding>& binding : _renderPasses)
		{
			passes.Add(binding.get());
		}

		return passes;
	}

	void RenderPipeline::Execute(RenderContext* renderContext)
	{
		for (uint i = 0; i < _renderPasses.Count(); i++)
		{
			Ref<IRenderPass> renderPass = _renderPasses[i]->GetRenderPass();

			renderContext->SetCurrentRenderPass(renderPass, i);

			try
			{
				renderPass->Execute(renderContext);
			}
			catch (const Exception& ex)
			{
				LogError(GetRenderingLogger(), FormattedString("Failed to execute \"{}\": {}", renderPass->GetName(), ex.what()));
			}
		}
	}

	void RenderPipeline::GatherPipelineAttachmentDescriptions()
	{
		_attachmentDescriptions.Clear();

		for (int rpI = 0; rpI < _renderPasses.Count(); rpI++)
		{
			const Managed<RenderPipelineBinding>& binding = _renderPasses[rpI];
			List<MappedAttachmentDescription> mappedPassAttachments = binding->GetMappedAttachmentDescriptions();

			for (int i = 0; i < mappedPassAttachments.Count(); i++)
			{
				const MappedAttachmentDescription& attachmentDescription = mappedPassAttachments[i];

				if (_attachmentDescriptions.Count() <= attachmentDescription.PipelineAttachmentIndex)
					_attachmentDescriptions.Resize(static_cast<uint64_t>(attachmentDescription.PipelineAttachmentIndex) + 1);

				RenderPipelineAttachmentDescription& pipelineAttachment = _attachmentDescriptions[attachmentDescription.PipelineAttachmentIndex];

				if (pipelineAttachment.Description == AttachmentDescription::Empty)
					pipelineAttachment.Description = attachmentDescription.AttachmentDescription;

				if (pipelineAttachment.Description != attachmentDescription.AttachmentDescription)
					throw RenderingOperationException("Conflicting render pipeline attachment binding");

				pipelineAttachment.AddPassUse(rpI);
			}
		}

		for (int i = 0; i < _attachmentDescriptions.Count(); i++)
		{
			if (_attachmentDescriptions[i].Description == AttachmentDescription::Empty)
				throw RenderingOperationException("Pipeline attachments must be contiguous");
		}

		_attachmentDescriptionsDirty = false;
	}
}
