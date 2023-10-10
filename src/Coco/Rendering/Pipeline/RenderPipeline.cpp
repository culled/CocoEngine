#include "Renderpch.h"
#include "RenderPipeline.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	std::hash<RenderPipeline*> _pipelineHasher;

	RenderPipeline::RenderPipeline() :
		_renderPasses{},
		_compiledPipeline(_pipelineHasher(this)),
		_isDirty(true)
	{}

	RenderPipeline::~RenderPipeline()
	{
		_renderPasses.clear();
	}

	bool RenderPipeline::AddRenderPass(SharedRef<RenderPass> renderPass, std::span<const uint8> bindingIndices)
	{
		std::span<const AttachmentFormat> passInputAttachments = renderPass->GetInputAttachments();

		if (bindingIndices.size() != passInputAttachments.size())
		{
			CocoError("Binding index count does not match the number of input attachments for the render pass ({} != {})", bindingIndices.size(), passInputAttachments.size())
			return false;
		}

		std::unordered_map<uint8, uint8> indexMapping;
		for (uint8 i = 0; i < bindingIndices.size(); i++)
			indexMapping[bindingIndices[i]] = i;

		_renderPasses.emplace_back(renderPass, indexMapping);

		MarkDirty();

		return true;
	}

	void RenderPipeline::RemoveRenderPass(const SharedRef<RenderPass>& renderPass)
	{
		auto it = std::find_if(_renderPasses.begin(), _renderPasses.end(), [renderPass](const RenderPassBinding& binding)
			{
				return renderPass.get() == binding.Pass.get();
			});

		if (it == _renderPasses.cend())
		{
			CocoError("RenderPass was not added to this RenderPipeline")
			return;
		}

		_renderPasses.erase(it);
		MarkDirty();
	}

	bool RenderPipeline::Compile()
	{
		CheckIfRenderPassesDirty();

		if (!_isDirty)
			return true;

		try
		{
			std::vector<RenderPassBinding> bindings;
			std::vector<std::optional<AttachmentFormat>> inputAttachments;
			std::vector<bool> clearAttachments;
			bool supportsMSAA = true;

			for (const RenderPassBinding& binding : _renderPasses)
			{
				if (!binding.PassEnabled)
					continue;

				std::span<const AttachmentFormat> passAttachments = binding.Pass->GetInputAttachments();

				if (!binding.Pass->SupportsMSAA())
					supportsMSAA = false;

				for (const auto& kvp : binding.PipelineToPassIndexMapping)
				{
					const uint8 pipelineAttachmentIndex = kvp.first;
					const uint8 passAttachmentIndex = kvp.second;

					if (pipelineAttachmentIndex < inputAttachments.size())
					{
						const std::optional<AttachmentFormat>& pipelineAttachment = inputAttachments.at(pipelineAttachmentIndex);

						if (pipelineAttachment.has_value() && !pipelineAttachment->IsCompatible(passAttachments[passAttachmentIndex]))
						{
							string err = FormatString(
								"Error compiling RenderPipeline: Attachment {} on RenderPass is incompatible with previously defined RenderPipeline attachment {}",
								passAttachmentIndex,
								pipelineAttachmentIndex);

							throw std::exception(err.c_str());
						}
					}
					else
					{
						inputAttachments.resize(static_cast<size_t>(pipelineAttachmentIndex) + 1);
						clearAttachments.resize(inputAttachments.size());
					}

					std::optional<AttachmentFormat>& pipelineAttachment = inputAttachments.at(pipelineAttachmentIndex);

					if (!pipelineAttachment.has_value())
					{
						pipelineAttachment = passAttachments[passAttachmentIndex];
						clearAttachments.at(passAttachmentIndex) = pipelineAttachment->ClearMode != AttachmentClearMode::Never;
					}
				}

				bindings.push_back(binding);
			}

			bool missingAttachments = std::any_of(inputAttachments.begin(), inputAttachments.end(), [](const std::optional<AttachmentFormat>& attachment)
				{
					return !attachment.has_value();
				});

			if (missingAttachments)
			{
				throw std::exception("The list of input attachments is not continuous");
			}

			_compiledPipeline.Version++;
			_compiledPipeline.RenderPasses = std::move(bindings);
			_compiledPipeline.ClearAttachments = std::move(clearAttachments);
			_compiledPipeline.SupportsMSAA = supportsMSAA;

			_compiledPipeline.InputAttachments.clear();
			std::transform(
				inputAttachments.begin(), 
				inputAttachments.end(), 
				std::back_inserter(_compiledPipeline.InputAttachments),
				[](std::optional<AttachmentFormat>& attachment)
				{
					return attachment.value();
				});

			_isDirty = false;
			return true;
		}
		catch (const std::exception& ex)
		{
			CocoError("Error compiling RenderPipeline: {}", ex.what())
			return false;
		}
	}

	void RenderPipeline::MarkDirty()
	{
		_isDirty = true;
	}

	void RenderPipeline::CheckIfRenderPassesDirty()
	{
		if (_compiledPipeline.RenderPasses.size() != _renderPasses.size())
		{
			MarkDirty();
			return;
		}

		for (uint64 i = 0; i < _renderPasses.size(); i++)
		{
			const RenderPassBinding& binding = _renderPasses.at(i);
			const RenderPassBinding& compiledBinding = _compiledPipeline.RenderPasses.at(i);

			if (binding.PassEnabled != compiledBinding.PassEnabled)
			{
				MarkDirty();
				return;
			}
		}
	}
}