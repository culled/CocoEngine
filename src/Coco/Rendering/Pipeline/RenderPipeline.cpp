#include "Renderpch.h"
#include "RenderPipeline.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	std::hash<RenderPipeline*> _pipelineHasher;

	RenderPipeline::RenderPipeline(const ResourceID& id, const string& name, AttachmentOptionFlags defaultAttachmentOptions) :
		Resource(id, name),
		_renderPasses{},
		_defaultAttachmentOptions(defaultAttachmentOptions),
		_attachmentOptions(),
		_compiledPipeline(id),
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

	void RenderPipeline::SetDefaultAttachmentOptions(AttachmentOptionFlags options)
	{
		_defaultAttachmentOptions = options;

		MarkDirty();
	}

	void RenderPipeline::SetAttachmentOptions(uint8 pipelineAttachmentIndex, AttachmentOptionFlags options)
	{
		_attachmentOptions[pipelineAttachmentIndex] = options;

		MarkDirty();
	}

	void RenderPipeline::RemoveAttachmentOptions(uint8 pipelineAttachmentIndex)
	{
		if (!_attachmentOptions.contains(pipelineAttachmentIndex))
			return;

		_attachmentOptions.erase(pipelineAttachmentIndex);

		MarkDirty();
	}

	AttachmentOptionFlags RenderPipeline::GetAttachmentOptions(uint8 pipelineAttachmentIndex) const
	{
		auto it = _attachmentOptions.find(pipelineAttachmentIndex);

		if (it != _attachmentOptions.end())
			return it->second;

		return _defaultAttachmentOptions;
	}

	bool RenderPipeline::Compile()
	{
		CheckIfRenderPassesDirty();

		if (!_isDirty)
			return true;

		try
		{
			std::vector<RenderPassBinding> bindings;
			_compiledPipeline.Attachments.clear();

			for (const RenderPassBinding& binding : _renderPasses)
			{
				if (!binding.PassEnabled)
					continue;

				std::span<const AttachmentFormat> passAttachments = binding.Pass->GetInputAttachments();

				for (const auto& kvp : binding.PipelineToPassIndexMapping)
				{
					const uint8 pipelineAttachmentIndex = kvp.first;
					const uint8 passAttachmentIndex = kvp.second;
					const AttachmentFormat& passAttachment = passAttachments[passAttachmentIndex];

					if (pipelineAttachmentIndex < _compiledPipeline.Attachments.size())
					{
						const CompiledPipelineAttachment& pipelineAttachment = _compiledPipeline.Attachments.at(pipelineAttachmentIndex);

						if (!pipelineAttachment.IsCompatible(passAttachment))
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
						_compiledPipeline.Attachments.resize(static_cast<size_t>(pipelineAttachmentIndex) + 1);
					}

					CompiledPipelineAttachment& pipelineAttachment = _compiledPipeline.Attachments.at(pipelineAttachmentIndex);

					if (pipelineAttachment == CompiledPipelineAttachment::Empty)
					{
						pipelineAttachment = CompiledPipelineAttachment(passAttachment, GetAttachmentOptions(pipelineAttachmentIndex));
					}
				}

				bindings.push_back(binding);
			}

			bool missingAttachments = std::any_of(_compiledPipeline.Attachments.begin(), _compiledPipeline.Attachments.end(), 
				[](const CompiledPipelineAttachment& attachment)
				{
					return attachment == CompiledPipelineAttachment::Empty;
				});

			if (missingAttachments)
			{
				throw std::exception("The list of input attachments is not continuous");
			}

			_compiledPipeline.Version++;
			_compiledPipeline.RenderPasses = std::move(bindings);

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