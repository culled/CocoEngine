#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include "IRenderPass.h"

namespace Coco::Rendering
{
	/// @brief A mapped attachment description
	struct MappedAttachmentDescription
	{
		/// @brief The attachment description
		AttachmentDescription AttachmentDescription = AttachmentDescription::Empty;

		/// @brief The index of the pipeline attachment
		uint PipelineAttachmentIndex = 0;
	};

	/// @brief A binding between an IRenderPass and a RenderPipeline
	class COCOAPI RenderPipelineBinding
	{
	private:
		Ref<IRenderPass> _renderPass;
		List<MappedAttachmentDescription> _mappedAttachmentDescriptions;

	public:
		RenderPipelineBinding(Ref<IRenderPass> renderPass, const List<int>& passToPipelineAttachmentMapping);
		virtual ~RenderPipelineBinding();

		/// @brief Gets the mapping between the pipeline attachments and this render pass's attachments
		/// @return The list of attachment mappings
		const List<MappedAttachmentDescription>& GetMappedAttachmentDescriptions() const noexcept { return _mappedAttachmentDescriptions; }

		/// @brief Gets the bound render pass
		/// @return The render pass
		Ref<IRenderPass> GetRenderPass() noexcept { return _renderPass; }
	};
}