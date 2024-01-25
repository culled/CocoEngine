#include "Renderpch.h"
#include "RenderPipeline.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderPipelineCompileException::RenderPipelineCompileException(const string& message) :
		Exception(message)
	{}

	RenderPipeline::RenderPipeline(const ResourceID& id, AttachmentOptionsFlags defaultAttachmentOptions) :
		Resource(id),
		_defaultAttachmentOptions(defaultAttachmentOptions),
		_renderPasses(),
		_compiledPipeline()
	{}

	bool RenderPipeline::AddRenderPass(SharedRef<RenderPass> renderPass, std::span<const uint32> pipelineAttachmentBindingIndices)
	{
		std::span<const RenderPassAttachment> passAttachments = renderPass->GetAttachments();

		if (pipelineAttachmentBindingIndices.size() != passAttachments.size())
		{
			CocoError("pipelineAttachmentBindingIndices size does not match the number of attachments for the render pass ({} != {})", 
				pipelineAttachmentBindingIndices.size(), passAttachments.size())
			return false;
		}

		std::unordered_map<uint32, uint32> pipelineAttachmentMapping;
		for (uint32 i = 0; i < pipelineAttachmentBindingIndices.size(); i++)
			pipelineAttachmentMapping[pipelineAttachmentBindingIndices[i]] = i;

		_renderPasses.emplace_back(renderPass, pipelineAttachmentMapping);

		MarkDirty();
		return true;
	}

	void RenderPipeline::RemoveRenderPass(const SharedRef<RenderPass>& renderPass)
	{
		auto it = std::find_if(_renderPasses.begin(), _renderPasses.end(), 
			[renderPass](const RenderPassPipelineBinding& binding)
			{
				return renderPass.get() == binding.Pass.get();
			});

		if(it == _renderPasses.end())
		{
			CocoError("RenderPass was not added to this RenderPipeline")
			return;
		}

		_renderPasses.erase(it);
		MarkDirty();
	}

	const CompiledRenderPipeline& RenderPipeline::GetOrCompile()
	{
		if (_compiledPipeline.has_value())
			return *_compiledPipeline;
		
		std::vector<std::optional<RenderPassAttachment>> pipelineAttachments;
		std::vector<RenderPassPipelineBinding> pipelinePasses;
		pipelinePasses.reserve(_renderPasses.size());

		for (const RenderPassPipelineBinding& pass : _renderPasses)
		{
			std::span<const RenderPassAttachment> passAttachments = pass.Pass->GetAttachments();

			for (const auto& kvp : pass.PipelineToPassAttachmentMapping)
			{
				const uint8 pipelineAttachmentIndex = kvp.first;
				const uint8 passAttachmentIndex = kvp.second;
				const RenderPassAttachment& passAttachment = passAttachments[passAttachmentIndex];

				if (pipelineAttachmentIndex < pipelineAttachments.size())
				{
					const auto& pipelineAttachment = pipelineAttachments.at(pipelineAttachmentIndex);

					// If the pipeline attachment is already defined, make sure it is compatible with the pass attachment
					if (pipelineAttachment.has_value() && !pipelineAttachment->IsCompatible(passAttachment))
					{
						throw RenderPipelineCompileException(
							FormatString(
								"Attachment {} on RenderPass \"{}\" is incompatible with previously defined RenderPipeline attachment {}",
								passAttachmentIndex,
								pass.Pass->GetName(),
								pipelineAttachmentIndex
							)
						);
					}
				}
				else
				{
					pipelineAttachments.resize(static_cast<size_t>(pipelineAttachmentIndex) + 1);
				}

				auto& pipelineAttachment = pipelineAttachments.at(pipelineAttachmentIndex);

				if (!pipelineAttachment.has_value())
				{
					pipelineAttachment.emplace(passAttachment);
				}
				else
				{
					pipelineAttachment->UsageFlags |= passAttachment.UsageFlags;
					pipelineAttachment->OptionsFlags |= passAttachment.OptionsFlags;
				}

				pipelineAttachment->OptionsFlags |= _defaultAttachmentOptions;
			}

			pipelinePasses.push_back(pass);
		}

		bool missingAttachments = std::any_of(pipelineAttachments.begin(), pipelineAttachments.end(),
			[](const auto& attachment)
			{
				return !attachment.has_value();
			});

		if (missingAttachments)
		{
			throw RenderPipelineCompileException("The list of pipeline attachments is not continuous");
		}

		CompiledRenderPipeline& compiledPipeline = _compiledPipeline.emplace(GetID(), GetVersion());
		compiledPipeline.RenderPasses = pipelinePasses;

		std::transform(pipelineAttachments.begin(), pipelineAttachments.end(),
			std::back_inserter(compiledPipeline.Attachments),
			[](const auto& attachment)
			{
				return attachment.value();
			});

		return compiledPipeline;
	}

	void RenderPipeline::MarkDirty()
	{
		_compiledPipeline.reset();
	}
}