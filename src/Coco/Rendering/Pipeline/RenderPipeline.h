#pragma once

#include "../RenderingResource.h"

#include <Coco/Core/Types/Color.h>
#include <Coco/Rendering/Graphics/Resources/RenderContext.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include "IRenderPass.h"
#include "RenderPipelineBinding.h"

namespace Coco::Rendering
{
	/// @brief A description for an attachment used by a RenderPipeline
	struct RenderPipelineAttachmentDescription
	{
		friend class RenderPipeline;

	public:
		/// @brief The attachment description
		AttachmentDescription Description = AttachmentDescription::Empty;

		/// @brief The index of the pass that this attachment is first used
		int FirstUsePassIndex = 0;

		/// @brief The index of the pass that this attachment is last used
		int LastUsePassIndex = 0;

		/// @brief If true, this attachment is used in the first pass of the pipeline
		bool IsUsedInFirstPipelinePass = false;

		/// @brief If true, this attachment is only used in a single render pass
		bool IsUsedInSinglePass = false;
		
	private:
		List<int> _passesUsed;

	public:
		RenderPipelineAttachmentDescription() = default;
		RenderPipelineAttachmentDescription(const AttachmentDescription& description) noexcept;
		~RenderPipelineAttachmentDescription();

		/// @brief Gets the indices of the render passes in a pipeline that this attachment is used
		/// @return The indices of the pipeline render passes where this attachment is used
		const List<int>& GetPassesUsed() const noexcept { return _passesUsed; }

	private:
		/// @brief Marks a pass's index where this attachment is used
		/// @param passIndex The index of the pass
		void AddPassUse(int passIndex);
	};

	/// @brief Defines a sequence of render passes to create a rendered image
	class COCOAPI RenderPipeline : public RenderingResource
	{
	private:
		List<ManagedRef<RenderPipelineBinding>> _renderPasses;
		List<RenderPipelineAttachmentDescription> _attachmentDescriptions;
		bool _attachmentDescriptionsDirty = false;
		Color _clearColor;

	public:
		RenderPipeline(ResourceID id, const string& name);
		~RenderPipeline() override;

		DefineResourceType(RenderPipeline)

		/// @brief Gets the render pipeline attachment descriptions for this pipeline
		/// @return This pipeline's attachment descriptions
		const List<RenderPipelineAttachmentDescription>& GetPipelineAttachmentDescriptions();

		/// @brief Adds a render pass to this pipeline with the given attachment mapping.
		/// The mapping list must equal the number of attachments the render pass uses.
		/// Each index correponds to the index of the pass attachment, and its value is the corresponding index in the pipeline's attachments
		/// @param renderPass The pass to add
		/// @param passToPipelineAttachmentBindings A mapping from the render pass's attachments to the pipeline's attachments
		/// @return A binding
		Ref<RenderPipelineBinding> AddRenderPass(SharedRef<IRenderPass> renderPass, const List<int>& passToPipelineAttachmentBindings);

		/// @brief Removes a binding and its render pass from this pipeline
		/// @param renderPassBinding The binding to the renderpass
		/// @return True if the binding was found and removed
		bool RemoveRenderPass(const SharedRef<RenderPipelineBinding>& renderPassBinding) noexcept;

		/// @brief Gets the list of render passes in this pipeline
		/// @return The passes in this pipeline
		List<Ref<RenderPipelineBinding>> GetPasses();

		/// @brief Sets the clear color of this render pipeline
		/// @param color The clear color
		void SetClearColor(const Color& color) noexcept { _clearColor = color; }

		/// @brief Gets the clear color used by this render pipeline
		/// @return The clear color
		Color GetClearColor() const noexcept { return _clearColor; }

		/// @brief Performs rendering operations for this pipeline
		/// @param renderContext The context to use for rendering
		void Execute(RenderContext& renderContext);

	private:
		/// @brief Updates the pipeline attachment descriptions to match the mapped attachments from this pipeline's render passes 
		void GatherPipelineAttachmentDescriptions();
	};
}