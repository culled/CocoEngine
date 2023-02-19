#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include "IRenderPass.h"
#include "RenderPipelineBinding.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A description for an attachment used by a RenderPipeline
	/// </summary>
	struct RenderPipelineAttachmentDescription
	{
	public:
		friend class RenderPipeline;

		AttachmentDescription Description = AttachmentDescription::Empty;

		int FirstUsePassIndex = 0;
		int LastUsePassIndex = 0;

		bool IsUsedInFirstPipelinePass = false;

		bool IsUsedInSinglePass = false;
		
	private:
		List<int> _passesUsed;

	public:
		RenderPipelineAttachmentDescription() = default;
		RenderPipelineAttachmentDescription(const AttachmentDescription& description);
		~RenderPipelineAttachmentDescription();

		const List<int>& GetPassesUsed() const { return _passesUsed; }

	private:
		void AddPassUse(int passIndex);
	};

	/// <summary>
	/// Defines a set of render passes to create a rendered image
	/// </summary>
	class COCOAPI RenderPipeline : public IRenderPass
	{
	private:
		List<Managed<RenderPipelineBinding>> _renderPasses;
		List<RenderPipelineAttachmentDescription> _attachmentDescriptions;
		bool _attachmentDescriptionsDirty = false;

	public:
		~RenderPipeline();

		virtual List<AttachmentDescription> GetAttachmentDescriptions() override;

		/// <summary>
		/// Gets the render pipeline attachment descriptions for this pipeline
		/// </summary>
		/// <returns>This pipeline's attachment descriptions</returns>
		List<RenderPipelineAttachmentDescription> GetPipelineAttachmentDescriptions();

		/// <summary>
		/// Adds a render pass to this pipeline with the given attachment mapping.
		/// The mapping list must equal the number of attachments the render pass uses.
		/// Each index correponds to the index of the pass attachment, and its value is the corresponding index in the pipeline's attachments
		/// </summary>
		/// <param name="renderPass">The pass to add</param>
		/// <param name="passToPipelineAttachmentBindings">A mapping from the render pass's attachments to the pipeline's attachments</param>
		/// <returns>A binding</returns>
		RenderPipelineBinding* AddRenderPass(Ref<IRenderPass> renderPass, const List<int>& passToPipelineAttachmentBindings);

		/// <summary>
		/// Removes a binding and its render pass from this pipeline
		/// </summary>
		/// <param name="renderPassBinding">The binding to the renderpass</param>
		/// <returns>True if the binding was found and removed</returns>
		bool RemoveRenderPass(RenderPipelineBinding* renderPassBinding);

	private:
		/// <summary>
		/// Updates the pipeline attachment descriptions to match the mapped attachments from this pipeline's render passes 
		/// </summary>
		void GatherPipelineAttachmentDescriptions();
	};
}