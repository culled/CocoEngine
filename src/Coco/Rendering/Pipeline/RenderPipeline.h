#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include "IRenderPass.h"
#include "RenderPipelineBinding.h"
#include <Coco/Core/Types/Color.h>
#include <Coco/Rendering/Graphics/RenderContext.h>

namespace Coco::Rendering
{
	/// <summary>
	/// A description for an attachment used by a RenderPipeline
	/// </summary>
	struct RenderPipelineAttachmentDescription
	{
	public:
		friend class RenderPipeline;

		/// <summary>
		/// The attachment description
		/// </summary>
		AttachmentDescription Description = AttachmentDescription::Empty;

		/// <summary>
		/// The index of the pass that this attachment is first used
		/// </summary>
		int FirstUsePassIndex = 0;

		/// <summary>
		/// The index of the pass that this attachment is last used
		/// </summary>
		int LastUsePassIndex = 0;

		/// <summary>
		/// If true, this attachment is used in the first pass of the pipeline
		/// </summary>
		bool IsUsedInFirstPipelinePass = false;

		/// <summary>
		/// If true, this attachment is only used in a single render pass
		/// </summary>
		bool IsUsedInSinglePass = false;
		
	private:
		List<int> _passesUsed;

	public:
		RenderPipelineAttachmentDescription() = default;
		RenderPipelineAttachmentDescription(const AttachmentDescription& description) noexcept;
		~RenderPipelineAttachmentDescription();

		/// <summary>
		/// Gets the indices of the render passes in a pipeline that this attachment is used
		/// </summary>
		/// <returns>The indices of the pipeline render passes where this attachment is used</returns>
		const List<int>& GetPassesUsed() const noexcept { return _passesUsed; }

	private:

		/// <summary>
		/// Marks a pass's index where this attachment is used
		/// </summary>
		/// <param name="passIndex"></param>
		void AddPassUse(int passIndex) noexcept;
	};

	/// <summary>
	/// Defines a set of render passes to create a rendered image
	/// </summary>
	class COCOAPI RenderPipeline
	{
	private:
		List<Managed<RenderPipelineBinding>> _renderPasses;
		List<RenderPipelineAttachmentDescription> _attachmentDescriptions;
		bool _attachmentDescriptionsDirty = false;
		Color _clearColor;

	public:
		RenderPipeline();
		virtual ~RenderPipeline();

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
		bool RemoveRenderPass(RenderPipelineBinding* renderPassBinding) noexcept;

		/// <summary>
		/// Gets the list of render passes in this pipeline
		/// </summary>
		/// <returns>The passes in this pipeline</returns>
		List<RenderPipelineBinding*> GetPasses() const;

		/// <summary>
		/// Sets the clear color of this render pipeline
		/// </summary>
		/// <param name="color">The clear color</param>
		void SetClearColor(const Color& color) noexcept { _clearColor = color; }

		/// <summary>
		/// Gets the clear color used by this render pipeline
		/// </summary>
		/// <returns>The clear color</returns>
		Color GetClearColor() const noexcept { return _clearColor; }

		/// <summary>
		/// Performs rendering operations for this pipeline
		/// </summary>
		/// <param name="renderContext">The context to use for rendering</param>
		void Execute(RenderContext* renderContext);

	private:
		/// <summary>
		/// Updates the pipeline attachment descriptions to match the mapped attachments from this pipeline's render passes 
		/// </summary>
		void GatherPipelineAttachmentDescriptions();
	};
}